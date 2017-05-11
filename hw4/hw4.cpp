#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <iostream>
#include <algorithm>

using namespace std;

class Process {
public:
  int process_id;
  int arrival_time;
  int burst_time;
  int original_burst_time; // used in RR
};

vector<Process*> processes;
int time_slice = 0; // lazy programming

bool compare_by_arrival(Process* left, Process* right) {
  return (left->arrival_time < right->arrival_time);
}

bool sjf_comp(Process* left, Process* right) {
  if (left->arrival_time <= right->arrival_time) {
    if (left->burst_time < right->burst_time)
      return true;
  }
  return false;
}

//Shortest Job First (nonpreemptive)
void sjf(double sim_time) {

  int current_time = 0;
  int throughput = 0; // num processes finished
  double total_wait_time = 0;
  double total_turn_around = 0;

  // sort by arrival time and burst time
  for (int i = 0; i < processes.size(); i++) {
    sort(processes.begin(), processes.end(), sjf_comp);
  }
  printf("=====================================================\n");

  for (int i = 0; i < processes.size(); i++) {

    total_wait_time += current_time - processes[i]->arrival_time;
    printf("%9d: scheduling PID %8d, CPU = %7d\n",current_time, processes[i]->process_id, processes[i]->burst_time);
    current_time += processes[i]->burst_time;
    if (current_time > sim_time) { // check if we are over
      current_time -= processes[i]->burst_time;
      break;
    }
    printf("%9d:            PID %8d terminated\n",current_time, processes[i]->process_id);
    total_turn_around += current_time - processes[i]->arrival_time;
    throughput++;
  }
  // printing
  printf("=====================================================\n");
  fprintf(stderr, "Throughput = %d\n", throughput);
  fprintf(stderr, "Avg wait time = %f\n", total_wait_time / processes.size());
  fprintf(stderr, "Avg turnaround time = %f\n", total_turn_around / processes.size());
  fprintf(stderr, "Remaining tasks = %lu\n", processes.size() - throughput);

}

// first come first serve
void fcfs(double sim_time) {
  // sort by arrival time
  for (int i = 0; i < processes.size(); i++) {
    sort(processes.begin(), processes.end(), compare_by_arrival);
  }

  int current_time = 0;
  double total_wait_time = 0;
  double total_turn_around = 0;
  int throughput = 0; // num processes finished

  printf("=====================================================\n");

  for (int i = 0; i < processes.size(); i++) {
    total_wait_time += current_time - processes[i]->arrival_time; // add to wait time
    printf("%9d: scheduling PID %8d, CPU = %7d\n",current_time, processes[i]->process_id, processes[i]->burst_time);
    current_time += processes[i]->burst_time;
    if (current_time > sim_time) { // check if we are over
      current_time -= processes[i]->burst_time;
      break;
    }
    printf("%9d:            PID %8d terminated\n",current_time, processes[i]->process_id);
    throughput++;
    total_turn_around += current_time - processes[i]->arrival_time; // add to total turnaround time
  }
  // printing
  printf("=====================================================\n");
  fprintf(stderr, "Throughput = %d\n", throughput);
  fprintf(stderr, "Avg wait time = %f\n", total_wait_time / processes.size());
  fprintf(stderr, "Avg turnaround time = %f\n", total_turn_around / processes.size());
  fprintf(stderr, "Remaining tasks = %lu\n", processes.size() - throughput);
}

// round robin
void rr(double sim_time) {

  int current_time = 0;
  int total_burst_time = 0;
  int throughput = 0; // num processes finished

  // sort by arrival time
  for (int i = 0; i < processes.size(); i++) {
    processes[i]->original_burst_time = processes[i]->burst_time; // init total burst times
    sort(processes.begin(), processes.end(), compare_by_arrival);
  }

  for (int i = 0; i < processes.size(); i++) { // init total_burst_time
    total_burst_time += processes[i]->burst_time;
  }

  printf("=====================================================\n");
  int i = 0;
  double total_turn_around = 0;
  double total_wait_time = 0;
  while (1) {

    if (processes[i]->burst_time == 0) {
        i = ++i % processes.size(); // reloop
        continue;
    }
    printf("%9d: scheduling PID %8d, CPU = %7d\n",current_time, processes[i]->process_id, processes[i]->burst_time);

      if (processes[i]->burst_time <= time_slice) { // burst <= time_slice
        current_time += processes[i]->burst_time;
        if (current_time > sim_time) { // check if we are over
          current_time -= processes[i]->burst_time;
          break;
        }
        total_burst_time -= processes[i]->burst_time;
        processes[i]->burst_time = 0;
        printf("%9d:            PID %8d terminated\n",current_time, processes[i]->process_id);
        throughput++;

        //wait time =  current time - arival_time - (original bust - burst time left)
        total_wait_time += current_time - processes[i]->arrival_time - processes[i]->original_burst_time;
        total_turn_around += current_time - processes[i]->arrival_time;
      }
      else if (processes[i]->burst_time > time_slice) { // burst < time_slice
        current_time += time_slice;
        if (current_time > sim_time) { // check if we are over
          current_time -= time_slice;
          break;
        }
        total_burst_time -= time_slice;
        processes[i]->burst_time -= time_slice;
        printf("%9d: suspending PID %8d, CPU = %7d\n",current_time, processes[i]->process_id, processes[i]->burst_time);
      }
      i = ++i % processes.size();
      if (total_burst_time < 1) // break if no total burst
        break;
  }

  printf("=====================================================\n");
  fprintf(stderr, "Throughput = %d\n", throughput);
  fprintf(stderr, "Avg wait time = %f\n", total_wait_time / processes.size());
  fprintf(stderr, "Avg turnaround time = %f\n", total_turn_around / processes.size());
  fprintf(stderr, "Remaining tasks = %lu\n", processes.size() - throughput);
}

int main(int argc, char **argv)
{
  if (argc < 3) {
    fprintf(stderr, "./a.out sim_time algorithm [time_slice]\n");
    exit(1);
  }

  int temp;
  int counter = 0;

  vector<int> tmpv;
  while (cin >> temp) {
    tmpv.push_back(temp); // store data in temp vector
  }

  for (int i = 0; i < tmpv.size()-2; i+=3) { // inc by 3 each time
    Process *process = new Process();
    process->process_id = tmpv[i];
    process->arrival_time = tmpv[i+1];
    process->burst_time = tmpv[i+2];
    processes.push_back(process); // push process back into our vect
  }

  double sim_time;
  string algorithm = "";
  try {
    sim_time = atoi(argv[1]);
    algorithm = argv[2];
  }
  catch (const std::exception& e) {
    fprintf(stderr, "usage: ./hw4 sim_time algo");
    exit(1);
  }

  if (algorithm == "SJF") {
    if (argc != 3){
      fprintf(stderr, "usage: ./hw4 sim_time SJF");
      exit(1);
    }
    sjf(sim_time);
  }
  if (algorithm == "FCFS") {
    if (argc != 3) {
      fprintf(stderr, "usage: ./hw4 sim_time FCFS");
      exit(1);
    }
    fcfs(sim_time);
  }
  if (algorithm == "RR") {
    if (argc != 4) {
      fprintf(stderr, "usage: ./hw4 sim_time RR time_slice");
      exit(1);
    }
    try {
      time_slice = atoi(argv[3]);
    }
    catch (const std::exception& e) {
      fprintf(stderr, "usage: ./hw4 sim_time RR time_slice");
      exit(1);
    }
    rr(sim_time);
  }
}
