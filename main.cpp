#include "process.h" // Defines IOEvent and Process
#include "ioModule.h" // Defines IOInterrupt and IOModule
#include "processMgmt.h" // Declares ProcessManagement

#include <chrono> // for sleep
#include <thread> // for sleep

/* TO DO:

1. Admitting new processes and labeling them as ready to run 
   (new -> ready transition)

2. Selecting an appropriate process to run from the set of ready processes 
   (we will just use a first in, first out scheduler -- ready -> running transition)

3. Handling running processes and what can happen while a process is running 
   (running -> blocked and running -> exit transitions)

4. Responding to interrupts that are raised when an IO operation is complete 
   (blocked -> ready transition)

*/

int main(int argc, char* argv[])
{
    // single thread processor
    // it's either processing something or it's not
//    bool processorAvailable = true;

    // vector of processes, processes will appear here when they are created by
    // the ProcessMgmt object (in other words, automatically at the appropriate time)
    vector<Process> processList;

    // this will orchestrate process creation in our system, it will add processes to 
    // processList when they are created and ready to be run/managed
    ProcessManagement processMgmt(processList);

    // this is where interrupts will appear when the ioModule detects that an IO operation is complete
    list<IOInterrupt> interrupts;   

    // this manages io operations and will raise interrupts to signal io completion
    IOModule ioModule(interrupts);  

    // Do not touch
    long time = 1;
    long sleepDuration = 50;
    string file;
    stringstream ss;
    enum stepActionEnum {noAct, admitNewProc, handleInterrupt, beginRun, continueRun, ioRequest, complete} stepAction;

    // Do not touch
    switch(argc)
    {
        case 1:
            file = "./procList.txt";  // default input file
            break;
        case 2:
            file = argv[1];         // file given from command line
            break;
        case 3:
            file = argv[1];         // file given
            ss.str(argv[2]);        // sleep duration given
            ss >> sleepDuration;
            break;
        default:
            cerr << "incorrect number of command line arguments" << endl;
            cout << "usage: " << argv[0] << " [file] [sleepDuration]" << endl;
            return 1;
            break;
    }

    processMgmt.readProcessFile(file);


	cout << "I made it here!\n";
    time = 0;
//    processorAvailable = true;

    // keep running the loop until all processes have been added and have run to completion
    // --> Running as long as there are still running processes
    while(processMgmt.moreProcessesComing() || !processList.empty())
    {
        //Update our current time step
        ++time;

        //let new processes in (ready) if there are any
        processMgmt.activateProcesses(time);

        //update the status for any active IO requests
        ioModule.ioProcessing(time);

        //If the processor is tied up running a process, then continue running it until it is done or blocks
        //   note: be sure to check for things that should happen as the process continues to run (io, completion...)
        //If the processor is free then you can choose the appropriate action to take, the choices (in order of precedence) are:
        // - admit a new process if one is ready (i.e., take a 'newArrival' process and put them in the 'ready' state)
        // - address an interrupt if there are any pending (i.e., update the state of a blocked process whose IO operation is complete)
        // - start processing a ready process if there are any ready


        //init the stepAction, update below
        stepAction = noAct;

        
        //TODO add in the code to take an appropriate action for this time step!
        //you should set the action variable based on what you do this time step. you can just copy and paste the lines below and uncomment them, if you want.
       

	cout << "I made it here!\n";

       // if there is a running process
        if(processList.front().state == processing){

            // update total time given to this process
            processList.front().processorTime++;
          
            // if the total time given to this processor matches the time at which the processors' first ioEvent occurs,
            // add the ioEvent to the pending list of all IOInterrupts
            if(processList.front().processorTime - processList.front().ioEvents.front().time == 0){
                ioModule.submitIORequest(time, processList.front().ioEvents.front(), processList.front());
                processList.front().state = blocked;
                stepAction = ioRequest;
            }
            // or if the process has run long enough to complete, mark it as so
            else if(processList.front().processorTime == processList.front().reqProcessorTime){
                processList.front().state = done;
                stepAction = complete;
            }
            else{ 
                stepAction = continueRun;
            }
        }
        // OR, if there is a newly added process with the state newArrival,
        // admit it by setting its state to ready
        else if (processList.back().state == newArrival){
	    int i=0;
            while(processList[i].state == ready) { i++; }
            processList[i].state = ready;
            stepAction = admitNewProc;
        }
        // OR, if there is an interrupt that has completed: 
        // find matching process to move to ready state and clear interrupt signal
        else if (!interrupts.empty()){
            for(Process& indexProc: processList){
                if(interrupts.front().procID == indexProc.id) {indexProc.state = ready;}
            }
            interrupts.pop_front();
            stepAction = handleInterrupt;
        }
        // OR, if the processList is not empty,
        // set the foremost process to running
        else if (!processList.empty()){
            // set first process to running
            processList.front().state = processing;
            stepAction = beginRun;
        }


        // Leave the below alone (at least for final submission, we are counting on the output being in expected format)
        cout << setw(5) << time << "\t"; 
        
        switch(stepAction)
        {
            case admitNewProc:
              cout << "[  admit]\t";
              break;
            case handleInterrupt:
              cout << "[ inrtpt]\t";
              break;
            case beginRun:
              cout << "[  begin]\t";
              break;
            case continueRun:
              cout << "[contRun]\t";
              break;
            case ioRequest:
              cout << "[  ioReq]\t";
              break;
            case complete:
              cout << "[ finish]\t";
              break;
            case noAct:
              cout << "[*noAct*]\t";
              break;
        }

        // You may wish to use a second vector of processes (you don't need to, but you can)
        printProcessStates(processList); // change processList to another vector of processes if desired

        this_thread::sleep_for(chrono::milliseconds(sleepDuration));
    }

    return 0;
}
