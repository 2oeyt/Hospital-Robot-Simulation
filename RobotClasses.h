#ifndef ROBOT_CLASSES_H
#define ROBOT_CLASSES_H

#include <string>
#include <vector>

// File 1: Class declarations (attributes, constructors, and methods).
// Method implementations are in main.cpp; test cases are in tests.cpp.

// Use simple strings for status values.
// Task: WAITING, IN_PROGRESS, DONE
// Robot: IDLE, MOVING, WORKING

class Task {
private:
    std::string name;
    std::string type;
    int x, y;
    int duration;
    int priority;
    int progress;
    std::string status;

public:
    Task(std::string name, std::string type, int x, int y,
         int duration, int priority);

    std::string getName() const;
    std::string getType() const;
    int getX() const;
    int getY() const;
    int getDuration() const;
    int getPriority() const;
    int getProgress() const;
    std::string getStatus() const;
    bool isWaiting() const;
    bool isDone() const;

    void start();
    void advance(int amount = 1); // Can advance faster for a change request.
};

class Robot {
private:
    std::string name;
    int x, y;
    int battery;
    int taskIndex;
    int completed;
    std::string status;

public:
    Robot(std::string name, int x, int y);
    virtual ~Robot(); // Virtual destructor for safe polymorphism.

    std::string getName() const;
    int getX() const;
    int getY() const;
    int getBattery() const;
    int getTaskIndex() const;
    int getCompleted() const;
    std::string getStatus() const;
    bool isIdle() const;

    virtual std::string supportedType() const = 0;
    virtual void work(Task& task) = 0;

    bool assignTask(int index, std::vector<Task>& tasks);
    void update(std::vector<Task>& tasks);
};

class DeliveryRobot : public Robot {
public:
    DeliveryRobot(std::string name, int x, int y);
    std::string supportedType() const override;
    void work(Task& task) override;
};

class CleanerRobot : public Robot {
public:
    CleanerRobot(std::string name, int x, int y);
    std::string supportedType() const override;
    void work(Task& task) override;
};

// Optional change-request examples; NOT used in the base scenario.
class FoodRobot : public Robot {
public:
    FoodRobot(std::string name, int x, int y);
    std::string supportedType() const override;
    void work(Task& task) override;
};

class FastDeliveryRobot : public DeliveryRobot {
public:
    FastDeliveryRobot(std::string name, int x, int y);
    void work(Task& task) override;
};

class Grid {
private:
    int width, height;

public:
    Grid(int width, int height);
    bool contains(int x, int y) const;
    void display(const std::vector<Robot*>& robots,
                 const std::vector<Task>& tasks) const;
};

class Simulation {
private:
    Grid grid;
    std::vector<Robot*> robots; // Simulation owns these raw pointers.
    std::vector<Task> tasks;
    int step;

    bool allDone() const;
    void assignTasks();

public:
    Simulation(int width = 10, int height = 5);
    ~Simulation(); // Delete robots created with new.
    Simulation(const Simulation&) = delete; // Prevent double delete.
    Simulation& operator=(const Simulation&) = delete;

    void addRobot(Robot* robot); // Takes ownership of the pointer.
    void addTask(std::string name, std::string type, int x, int y,
                 int duration, int priority);

    int getStep() const;
    int robotCount() const;
    int taskCount() const;
    const Robot& getRobot(int index) const;
    const Task& getTask(int index) const;
    bool finished() const;

    bool tick();
    void display() const;
    void run(int maxSteps = 200);
};

void setupHospitalScenario(Simulation& sim);

#endif // ROBOT_CLASSES_H
