#ifndef TASK_LIBRARY_H
#define TASK_LIBRARY_H

#include <functional>
#include <set>
#include <utility>

namespace TaskParallelize {
    namespace {
        int task_universe_count = 0;
        class globalTask {
            int universe_id;
            int task_id;
            std::set<globalTask*> required_tasks;
            std::set<globalTask*> enabling_tasks;
            std::function<void(void)> task_function;
        public:
            globalTask(int universe_id, int task_id,std::function<void(void)> func);
            void addRequirement(globalTask* required_task);
            void addEnabler(globalTask* enabling_task);
            inline int getUniverseId(){ return universe_id;}
            inline int getTaskId(){ return task_id;}
            inline std::set<globalTask*> getRequiredTasks(){ return required_tasks;}
            inline std::set<globalTask*> getEnablingTasks(){ return enabling_tasks;}
            inline std::function<void()> getTaskFunction(){return task_function;}
        };
    }
    class task {
        globalTask* task_pointer;
        task(int universe_id,int task_id,std::function<void(void)> task_function);
        friend class taskUniverse;
    public:
        void then(task t);
    };
    class taskUniverse {
        int universe_id;
        int task_count;
        std::vector<globalTask*> tasks;
        void verifyAcyclic();
        void computeParallelSets(std::vector<std::vector<globalTask*>> &parallel_sets);
        static void kosarajuDFS(globalTask* cur_task,std::stack<globalTask*> &nodes,std::set<int> &visited);
        static void parallelSetDFS(globalTask* cur_task,int level, std::vector<std::vector<globalTask*>> &parallel_sets,std::map<globalTask*,int> &enabling_count);
        static void runParallelSets(std::vector<std::vector<globalTask*>> &parallel_sets);
    public:
        taskUniverse();
        task createTask(std::function<void(void)>);
        void runUniverse();
    };

    globalTask::globalTask(int universe_id, int task_id, std::function<void(void)> func) {
        this->universe_id = universe_id;
        this->task_id = task_id;
        this->task_function = std::move(func);
    }

    void globalTask::addRequirement(globalTask *required_task) {
        required_tasks.insert(required_task);
    }

    void globalTask::addEnabler(globalTask *enabling_task) {
        enabling_tasks.insert(enabling_task);
    }
}

#endif //TASK_LIBRARY_H