#include <stack>
#include <map>
#include <future>
#include <utility>
#include "library.h"

TaskParallelize::task::task(int universe_id, int task_id, std::function<void(void)> task_function) {
    task_pointer = new globalTask(universe_id,task_id,std::move(task_function));
}

void TaskParallelize::task::then(TaskParallelize::task t) {
    if(t.task_pointer->getUniverseId() != task_pointer->getUniverseId())
        throw std::invalid_argument("Tasks must belong to the same universe");
    t.task_pointer->addRequirement(this->task_pointer);
    this->task_pointer->addEnabler(t.task_pointer);
}

TaskParallelize::taskUniverse::taskUniverse() {
    universe_id = task_universe_count++;
    task_count = 0;
    tasks = std::vector<globalTask*>();
}

TaskParallelize::task TaskParallelize::taskUniverse::createTask(std::function<void(void)> func) {
    task t = TaskParallelize::task(universe_id, task_count++, std::move(func));
    tasks.emplace_back(t.task_pointer);
    return t;
}

void TaskParallelize::taskUniverse::runUniverse() {
    verifyAcyclic();
    std::vector<std::vector<globalTask*>> parallel_sets;
    computeParallelSets(parallel_sets);
    runParallelSets(parallel_sets);
}

void TaskParallelize::taskUniverse::verifyAcyclic() {
    std::stack<globalTask*> nodes;
    std::set<int> visited;
    for(globalTask* current_task:tasks){
        if(visited.find(current_task->getTaskId())==visited.end()){
            kosarajuDFS(current_task,nodes,visited);
        }
    }
    visited.clear();
    while(!nodes.empty()){
        globalTask* current_task = nodes.top();
        nodes.pop();
        std::set<globalTask*> children_tasks = current_task->getEnablingTasks();
        for(globalTask* children_task:children_tasks){
            if(visited.find(children_task->getTaskId()) == visited.end())
                throw std::logic_error("Tasks cannot have cyclic dependencies");
        }
        visited.insert(current_task->getTaskId());
    }
}

void TaskParallelize::taskUniverse::kosarajuDFS(TaskParallelize::globalTask *cur_task, std::stack<globalTask*> &nodes,
                                         std::set<int> &visited) {
    if(visited.find(cur_task->getTaskId())!=visited.end())
        return;
    visited.insert(cur_task->getTaskId());
    std::set<globalTask *> parent_tasks = cur_task->getRequiredTasks();
    for(globalTask* parent_task:parent_tasks){
        kosarajuDFS(parent_task,nodes,visited);
    }
    nodes.push(cur_task);
}

void TaskParallelize::taskUniverse::computeParallelSets(std::vector<std::vector<globalTask *>> &parallel_sets) {
    std::map<globalTask*,int> enabling_count;
    for(globalTask* cur_task: tasks){
        enabling_count[cur_task] = cur_task->getEnablingTasks().size();
    }
    for(globalTask* cur_task: tasks){
        if(cur_task->getEnablingTasks().empty()){
            parallelSetDFS(cur_task,0,parallel_sets,enabling_count);
        }
    }
}

void TaskParallelize::taskUniverse::parallelSetDFS(TaskParallelize::globalTask *cur_task, int level,
                                                   std::vector<std::vector<globalTask *>> &parallel_sets,
                                                   std::map<globalTask*,int> &enabling_count) {
    if(enabling_count[cur_task] > 0)
        return;
    else if(enabling_count[cur_task] < 0)
        throw std::runtime_error("Internal error occurred");
    if(level > parallel_sets.size())
        throw std::runtime_error("Internal error occurred");
    if(level == parallel_sets.size())
        parallel_sets.emplace_back(std::vector<globalTask*>());
    parallel_sets[level].emplace_back(cur_task);
    std::set<globalTask*> required_tasks = cur_task->getRequiredTasks();
    for(globalTask* parent_task:required_tasks){
        enabling_count[parent_task]--;
    }
    for(globalTask* parent_task:required_tasks){
        parallelSetDFS(parent_task,level + 1,parallel_sets,enabling_count);
    }
}

void TaskParallelize::taskUniverse::runParallelSets(std::vector<std::vector<globalTask *>> &parallel_sets) {
    for(int i=parallel_sets.size()-1;i>=0;i--){
        std::vector<std::future<void>> futures;
        for(auto & j : parallel_sets[i]){
            futures.emplace_back(std::async(j->getTaskFunction()));
        }
        for(std::future<void> &cur_task:futures)
            cur_task.get();
    }
}
