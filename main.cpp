#include "RobotClasses.h"

#include <iostream>

// File 2: Method implementations, scenario setup, and main().
// Status values use std::string, and standard-library names use std::.

// ========== TASK ==========
Task::Task(std::string name, std::string type, int x, int y,
           int duration, int priority)
    : name(name), type(type), x(x), y(y), duration(duration),
      priority(priority), progress(0), status("WAITING") {
    // Keep task data valid without throwing exceptions.
    if (this->duration <= 0) {
        std::cerr << "Invalid duration, using 1 instead.\n";
        this->duration = 1;
    }
    if (this->priority <= 0) {
        std::cerr << "Invalid priority, using 1 instead.\n";
        this->priority = 1;
    }
}

std::string Task::getName() const { return name; }
std::string Task::getType() const { return type; }
int Task::getX() const { return x; }
int Task::getY() const { return y; }
int Task::getDuration() const { return duration; }
int Task::getPriority() const { return priority; }
int Task::getProgress() const { return progress; }
std::string Task::getStatus() const { return status; }
bool Task::isWaiting() const { return status == "WAITING"; }
bool Task::isDone() const { return status == "DONE"; }

void Task::start() {
    if (isWaiting()) status = "IN_PROGRESS";
}

void Task::advance(int amount) {
    if (status != "IN_PROGRESS" || amount <= 0) return;
    if (amount >= duration - progress) {
        progress = duration;
        status = "DONE";
    } else {
        progress += amount;
    }
}

// ========== ROBOT (BASE CLASS) ==========
Robot::Robot(std::string name, int x, int y)
    : name(name), x(x), y(y), battery(100), taskIndex(-1),
      completed(0), status("IDLE") {}

Robot::~Robot() = default;

std::string Robot::getName() const { return name; }
int Robot::getX() const { return x; }
int Robot::getY() const { return y; }
int Robot::getBattery() const { return battery; }
int Robot::getTaskIndex() const { return taskIndex; }
int Robot::getCompleted() const { return completed; }
std::string Robot::getStatus() const { return status; }
bool Robot::isIdle() const { return taskIndex == -1; }

bool Robot::assignTask(int index, std::vector<Task>& tasks) {
    if (!isIdle() || index < 0 || index >= (int)tasks.size()) return false;

    Task& task = tasks[index];
    if (!task.isWaiting() || task.getType() != supportedType()) return false;

    taskIndex = index;
    task.start();
    status = "MOVING";
    return true;
}

void Robot::update(std::vector<Task>& tasks) {
    if (isIdle()) {
        status = "IDLE";
        return;
    }

    Task& task = tasks.at(taskIndex);

    if (x != task.getX() || y != task.getY()) {
        status = "MOVING";

        // Only ONE cell each timestep. Horizontal first, then vertical.
        if (x < task.getX()) ++x;
        else if (x > task.getX()) --x;
        else if (y < task.getY()) ++y;
        else if (y > task.getY()) --y;
    } else {
        status = "WORKING";
        work(task); // Polymorphism: use the child class implementation.

        if (task.isDone()) {
            ++completed;
            taskIndex = -1;
            status = "IDLE";
        }
    }

    // Battery is a display value in the base project, not a stop condition.
    if (battery > 0) --battery;
}

// ========== DERIVED ROBOTS ==========
DeliveryRobot::DeliveryRobot(std::string name, int x, int y)
    : Robot(name, x, y) {}

std::string DeliveryRobot::supportedType() const { return "Delivery"; }

void DeliveryRobot::work(Task& task) {
    std::cout << "  " << getName() << " delivers: " << task.getName() << '\n';
    task.advance();
}

CleanerRobot::CleanerRobot(std::string name, int x, int y)
    : Robot(name, x, y) {}

std::string CleanerRobot::supportedType() const { return "Cleaning"; }

void CleanerRobot::work(Task& task) {
    std::cout << "  " << getName() << " cleans: " << task.getName() << '\n';
    task.advance();
}

// Optional examples of change requests.
FoodRobot::FoodRobot(std::string name, int x, int y)
    : Robot(name, x, y) {}

std::string FoodRobot::supportedType() const { return "Food"; }

void FoodRobot::work(Task& task) {
    std::cout << "  " << getName() << " delivers food: " << task.getName() << '\n';
    task.advance();
}

FastDeliveryRobot::FastDeliveryRobot(std::string name, int x, int y)
    : DeliveryRobot(name, x, y) {}

void FastDeliveryRobot::work(Task& task) {
    std::cout << "  " << getName() << " delivers faster: " << task.getName() << '\n';
    task.advance(getBattery() == 100 ? 2 : 1);
}

// ========== GRID ==========
Grid::Grid(int width, int height) : width(width), height(height) {
    if (this->width < 10) {
        std::cerr << "Grid width must be at least 10; using 10.\n";
        this->width = 10;
    }
    if (this->height < 5) {
        std::cerr << "Grid height must be at least 5; using 5.\n";
        this->height = 5;
    }
}

bool Grid::contains(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

void Grid::display(const std::vector<Robot*>& robots,
                   const std::vector<Task>& tasks) const {
    std::vector<std::string> cells(height, std::string(width, '.'));

    for (const Task& task : tasks) {
        if (!task.isDone()) cells[task.getY()][task.getX()] = '*';
    }
    for (int i = 0; i < (int)robots.size(); ++i) {
        char label = (i < 9) ? char('1' + i) : '?';
        cells[robots[i]->getY()][robots[i]->getX()] = label;
    }

    std::cout << "   ";
    for (int x = 0; x < width; ++x) std::cout << x % 10 << ' ';
    std::cout << '\n';
    for (int y = 0; y < height; ++y) {
        std::cout << y << "  ";
        for (int x = 0; x < width; ++x) std::cout << cells[y][x] << ' ';
        std::cout << '\n';
    }
    std::cout << "* = unfinished task | ";
    for (int i = 0; i < (int)robots.size(); ++i)
        std::cout << (i < 9 ? std::to_string(i + 1) : "?") << '='
             << robots[i]->getName() << ' ';
    std::cout << '\n';
}

// ========== SIMULATION ==========
Simulation::Simulation(int width, int height)
    : grid(width, height), step(0) {}

Simulation::~Simulation() {
    for (Robot* robot : robots) {
        delete robot; // Robot's virtual destructor handles derived classes.
    }
}

void Simulation::addRobot(Robot* robot) {
    if (robot == nullptr) {
        std::cerr << "Cannot add a null robot.\n";
        return;
    }
    if (!grid.contains(robot->getX(), robot->getY())) {
        std::cerr << "Robot starting position outside grid.\n";
        delete robot; // Release rejected robot.
        return;
    }
    robots.push_back(robot);
}

void Simulation::addTask(std::string name, std::string type, int x, int y,
                         int duration, int priority) {
    if (!grid.contains(x, y)) {
        std::cerr << "Task position outside grid.\n";
        return;
    }
    if (duration <= 0 || priority <= 0) {
        std::cerr << "Task duration and priority must be positive.\n";
        return;
    }
    tasks.emplace_back(name, type, x, y, duration, priority);
}

int Simulation::getStep() const { return step; }
int Simulation::robotCount() const { return (int)robots.size(); }
int Simulation::taskCount() const { return (int)tasks.size(); }
const Robot& Simulation::getRobot(int index) const { return *robots.at(index); }
const Task& Simulation::getTask(int index) const { return tasks.at(index); }
bool Simulation::finished() const { return allDone(); }

bool Simulation::allDone() const {
    for (const Task& task : tasks)
        if (!task.isDone()) return false;
    return true;
}

void Simulation::assignTasks() {
    for (Robot* robot : robots) {
        if (!robot->isIdle()) continue;

        int best = -1;
        for (int i = 0; i < (int)tasks.size(); ++i) {
            if (!tasks[i].isWaiting()) continue;
            if (tasks[i].getType() != robot->supportedType()) continue;

            // Extra feature: choose the highest priority (large number first).
            if (best == -1 || tasks[i].getPriority() > tasks[best].getPriority())
                best = i;
        }
        if (best != -1) robot->assignTask(best, tasks);
    }
}

bool Simulation::tick() {
    if (allDone()) return false;
    ++step;
    assignTasks();

    for (Robot* robot : robots)
        robot->update(tasks);
    return true;
}

void Simulation::display() const {
    std::cout << "\n========== STEP " << step << " ==========\n";
    grid.display(robots, tasks);

    std::cout << "ROBOTS:\n";
    for (Robot* robot : robots) {
        std::cout << "  " << robot->getName() << " [" << robot->supportedType() << "] ("
             << robot->getX() << ',' << robot->getY() << ") "
             << robot->getStatus()
             << " battery=" << robot->getBattery() << "%";
        if (!robot->isIdle())
            std::cout << " task=" << tasks[robot->getTaskIndex()].getName();
        std::cout << '\n';
    }

    std::cout << "TASKS:\n";
    for (const Task& task : tasks) {
        std::cout << "  " << task.getName() << " [" << task.getType()
             << ", Priority=" << task.getPriority() << "] "
             << task.getStatus() << ' '
             << task.getProgress() << '/' << task.getDuration() << '\n';
    }
}

void Simulation::run(int maxSteps) {
    display();
    while (!allDone() && step < maxSteps) {
        tick();
        display();
    }

    int done = 0;
    for (const Task& task : tasks)
        if (task.isDone()) ++done;

    std::cout << "\n========== SUMMARY ==========\n";
    std::cout << "Total timesteps: " << step << '\n';
    std::cout << "Completed tasks: " << done << '/' << tasks.size() << '\n';
    for (Robot* robot : robots)
        std::cout << robot->getName() << " completed "
             << robot->getCompleted() << " task(s)\n";

    if (!allDone()) std::cout << "WARNING: unfinished tasks / timestep limit reached\n";
}

// ========== SCENARIO SETUP ==========
void setupHospitalScenario(Simulation& sim) {

    // 3 robots, 2 types (solo-project requirements).
    sim.addRobot(new DeliveryRobot("D1", 0, 0));
    sim.addRobot(new DeliveryRobot("D2", 1, 0));
    sim.addRobot(new CleanerRobot("C1", 0, 4));

    // 5 tasks, 2 types. (name, type, x, y, duration, priority)
    sim.addTask("Medicine A", "Delivery", 4, 1, 2, 3);
    sim.addTask("Medicine B", "Delivery", 9, 2, 3, 1);
    sim.addTask("Medicine C", "Delivery", 3, 4, 2, 2);
    sim.addTask("Room A", "Cleaning", 7, 0, 3, 3);
    sim.addTask("Room B", "Cleaning", 8, 4, 2, 1);

}

// Functions defined in tests.cpp (unit tests and change-request demos).
void runTests();
void runFoodDemo();
void runFastDemo();

// ========== MAIN ==========
int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--test") {
        runTests();
    } else if (argc > 1 && std::string(argv[1]) == "--food") {
        runFoodDemo();
    } else if (argc > 1 && std::string(argv[1]) == "--fast") {
        runFastDemo();
    } else if (argc == 1) {
        Simulation sim;
        setupHospitalScenario(sim);
        sim.run();
    } else {
        std::cerr << "Usage: ./hospital [--test | --food | --fast]\n";
        return 1;
    }
    return 0;
}
