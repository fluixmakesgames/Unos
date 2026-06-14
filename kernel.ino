#include <EEPROM.h>

#define MAX_SIZE 9
#define MAX_TASKS 8

#include "fs.h"

typedef struct {
  char name[MAX_SIZE];
  bool active;
  void (*entry)(int);
}Task;

Task Tasks[MAX_TASKS];
unsigned long Ticks = 0; // Unsigned Long to prevent overflowing for ~50 days.

/*
SpawnTask (
  name -> names the task process with a max size of 8 characters (+1 for null term),
  entry -> function pointer to run the task.
)

Returns ID of the task, or -1 if failed to spawn the task.
*/
int SpawnTask(const char* name, void (*entry)(int)) { 
    for(int i = 0; i < MAX_TASKS; i++) { 
        if(!Tasks[i].active) { 
            Tasks[i].active = true; 
            strncpy(Tasks[i].name, name, MAX_SIZE - 1);
            Tasks[i].name[MAX_SIZE - 1] = '\0';
            Tasks[i].entry = entry;
            return i;
        } 
    } 
    return -1;
}

/*
KillTask(
  id -> id of the specified task to kill
)

Sets the active tag of a task to false so it may stop running, and be overwritten.
*/
void KillTask(int id) {
    if(id >= 0 && id < MAX_TASKS) {
        Tasks[id].active = false;
    }
}

/*
TaskMonitor()

Monitors every task with its ID and name.
*/
void TaskMonitor() {
    Serial.println("PID | Name");

    for(int i = 0; i < MAX_TASKS; i++) {
        if(Tasks[i].active) {
            Serial.print(i);
            Serial.print("   | ");
            Serial.print(Tasks[i].name);
            Serial.println("");
        }
    }
}

/*
RunTasks()

Runs every active task.
*/
void RunTasks() {
  for(int i = 0; i < MAX_TASKS; i++) { // Loops through every task
    if(Tasks[i].active) {
      Tasks[i].entry(i); // Runs task
    }
  }
}

/*
freeMem()

Returns the bytes of free RAM
*/
int freeMem() {
  char top;
  extern char *__brkval;
  extern char __bss_end;
  return __brkval == 0 ? &top - &__bss_end : &top - __brkval;
}

/*
Idle()

Hogs the CPU until serial wakes up to have a "low power" mode.
*/
void Idle(int pid) {
  while(Serial.available() == 0) {
      // Do absolutely nothing.
  }
  KillTask(pid);
  SpawnTask("Shell",shell);
  Serial.println("Leaving idle mode.");
}

/*
shell()

Main user shell for running commands.
*/
void shell(int pid) {
  if (Serial.available() > 0) {
    String Com = Serial.readString(); // Reads string if the user sent one.
    int ArgIndex = Com.indexOf(' '); // Gets index of first space for later.

    Serial.println(""); // Makes newline automatically for PuTTY terminals

    Com.trim(); // Trims newline from command.
    if(Com == "uptime") {
      if((Ticks / 60000) < 10) Serial.print("0");
      Serial.print(Ticks / 60000);
      Serial.print(":");
      if((Ticks / 1000) % 60 < 10) Serial.print("0");
      Serial.print((Ticks / 1000) % 60);
    } else if(Com == "fmem") {
      Serial.println(freeMem());
    } else if(Com == "taskmon") {
      TaskMonitor();
    } else if(Com == "rep") {
      for(int i = 0; i < 1024; i++) {
        Serial.print(EEPROM[i]);
        Serial.print(",");
      }
    } else if(Com == "beep") {
      Serial.print("\a"); // Beeps if you use PuTTY
    } else if(Com == "ls") {
      for(int i = 0; i < 4; i++) {
        if(!FileExistsAtIndex(i)) continue;
        char fname[MAX_SIZE];
        R_GFName(i,fname);
        Serial.println(fname);
      }
    } else if(Com == "sleep") {
      KillTask(pid);
      SpawnTask("Sleeper", Idle);
    } else if(Com == "help") {
      Serial.println("help    | Lists Commands.");
      Serial.println("uptime  | Displays uptime.");
      Serial.println("taskmon | Runs the task monitor.");
      Serial.println("echo    | Echos users input.");
      Serial.println("kill    | Kills specified task.");
      Serial.println("fmem    | Shows amount of free RAM.");
      Serial.println("sleep   | Idles the kernel.");
      Serial.println("ls      | Lists each file.");
      Serial.println("touch   | Creates file.");
      Serial.println("cat     | Echos file contents.");
      Serial.println("rm      | Destroys specified file.");
      Serial.println("rep     | Reads EEPROM as numbers.");
    } else {
      // Checks if theres no arguments to skip cutting the string and just safely error out.
      if(ArgIndex <= -1) {
        Serial.println("Bad Com.");
        return;
      }

      // Cuts string from start to space
      String LCom = Com.substring(0, ArgIndex);
      // Cuts string from argument
      String Arg = Com.substring(ArgIndex+1);

      // Argument commands
      if(LCom == "echo") {
        Serial.println(Arg);
      } else if(LCom == "touch") {
        CreateFile(Arg.c_str());
      } else if(LCom == "rm") {
        DestroyFile(Arg.c_str());
      } else if(LCom == "cat") {
        if(!FileExists(Arg.c_str())) {
          Serial.println("Non-Existent File!");
          return;
        }
        char dat[ReadFileSize(Arg.c_str())];
        ReadFile(Arg.c_str(),dat);
        Serial.println(dat);
      } else if(LCom == "kill") {
        if(Arg.toInt() >= MAX_TASKS) {
          Serial.println("Non-Existent Task!");
        }

        if(Arg.toInt() < 0) {
          Serial.println("Non-Existent Task!");
        }
        KillTask(Arg.toInt());
      } else Serial.println("Bad Com.");
    }
  } else return;
}

void setup() {
  // Begins serial
  Serial.begin(9600);
  // Notifys user of serial beginning
  Serial.println("[X] Serial began on 9600");
  // Spawns shell and notifys user of it
  SpawnTask("Shell",shell);
  Serial.println("[X] Shell task spawned");

  // After Kernel Setup
  Serial.println("Unos kernel v 0.2");
  Serial.println("(2026)");
}

void loop() {
  // Runs all of the tasks and upticks by 1
  RunTasks();

  Ticks = millis();
}
