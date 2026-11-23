#include "RobotClasses.h"

#include <iostream>
#include <cstdlib>

// File 3: unit tests and change-request demo scenarios.
// All Robot/Task/Simulation method implementations stay in main.cpp.

// ========== UNIT TESTS (SEPARATE FILE) ==========
// Run: ./hospital --test (compile main.cpp and tests.cpp together)
void check(bool condition, const std::string& message) {
    if (!condition) {
        std::cout << "TEST FAILED: " << message << '\n';
        std::exit(1);
    }
}

void runTests() {
    // 1. Task constructor, starting, and progress.
    Task t("Test", "Delivery", 2, 1, 2, 3);
    check(t.isWaiting() && t.getProgress() == 0, "Task initial state");
    t.advance();
    check(t.getProgress() == 0, "Cannot work before start");
    t.start();
    t.advance();
    check(t.getProgress() == 1, "Task progress");
    t.advance(10);
    check(t.isDone() && t.getProgress() == 2, "Task completion");

    // 2. One-cell movement (horizontal before vertical).
    std::vector<Task> tasks;
    tasks.emplace_back("Medicine", "Delivery", 2, 1, 1, 2);
    DeliveryRobot d("D-test", 0, 0);
    check(d.assignTask(0, tasks), "Robot takes matching task");
    d.update(tasks);
    check(d.getX() == 1 && d.getY() == 0, "First horizontal step");
    d.update(tasks);
    check(d.getX() == 2 && d.getY() == 0, "Second horizontal step");
    d.update(tasks);
    check(d.getX() == 2 && d.getY() == 1, "Vertical step");
    d.update(tasks);
    check(tasks[0].isDone() && d.getCompleted() == 1, "Robot completion");
    check(d.getBattery() == 96, "Battery decreased 4 times");

    // 3. Robot cannot take a different type of task.
    std::vector<Task> cleaning;
    cleaning.emplace_back("Room", "Cleaning", 0, 0, 1, 1);
    check(!d.assignTask(0, cleaning), "Reject wrong task type");
    CleanerRobot c("C-test", 0, 0);
    check(c.assignTask(0, cleaning), "Cleaner accepts cleaning task");
    c.update(cleaning);
    check(cleaning[0].isDone(), "Cleaner works");

    // 4. Priority system chooses larger priority first.
    Simulation prioritySim;
    prioritySim.addRobot(new DeliveryRobot("P1", 0, 0));
    prioritySim.addTask("Low", "Delivery", 5, 0, 1, 1);
    prioritySim.addTask("High", "Delivery", 4, 0, 1, 3);
    prioritySim.tick();
    check(prioritySim.getTask(0).isWaiting(), "Low priority waits");
    check(prioritySim.getRobot(0).getTaskIndex() == 1, "High priority first");

    // 5. Add a new robot/task type without modifying Simulation.
    Simulation foodSim;
    setupHospitalScenario(foodSim);
    foodSim.addRobot(new FoodRobot("F1", 1, 2));
    foodSim.addTask("Food A", "Food", 1, 2, 1, 3);
    foodSim.tick();
    check(foodSim.robotCount() == 4 && foodSim.taskCount() == 6,
          "New type added");
    check(foodSim.getTask(5).isDone(), "FoodRobot completes food task");

    // 6. Fast robot works by 2 units only at full battery.
    Simulation speedSim;
    speedSim.addRobot(new FastDeliveryRobot("F2", 2, 2));
    speedSim.addTask("Urgent", "Delivery", 2, 2, 4, 3);
    speedSim.tick();
    check(speedSim.getTask(0).getProgress() == 2, "Double progress at full battery");
    speedSim.tick();
    check(speedSim.getTask(0).getProgress() == 3, "Normal progress afterward");
    speedSim.tick();
    check(speedSim.getTask(0).isDone(), "Fast robot completes task");

    // 7. Base scenario: 3 robots, 5 tasks, finished in 21 steps.
    Simulation base;
    setupHospitalScenario(base);
    check(base.robotCount() == 3 && base.taskCount() == 5,
          "Solo project size requirements");
    for (int i = 0; i < 200 && !base.finished(); ++i) base.tick();
    check(base.finished() && base.getStep() == 21, "Base scenario completed");

    std::cout << "All 7 test groups passed!\n";
}


// ========== CHANGE REQUEST DEMOS ==========
void runFoodDemo() {
    Simulation sim;
    setupHospitalScenario(sim);
    sim.addRobot(new FoodRobot("F1", 1, 2));
    sim.addTask("Food A", "Food", 1, 2, 1, 3);
    sim.run();
}

void runFastDemo() {
    Simulation sim;
    sim.addRobot(new FastDeliveryRobot("F2", 2, 2));
    sim.addTask("Urgent", "Delivery", 2, 2, 4, 3);
    sim.run();
}
