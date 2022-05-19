package ija.ija2020.proj.calendar;

import java.time.LocalTime;
import java.util.function.Consumer;
import java.util.function.Function;

/**
 * Event to be used with Calendar
 * @see Calendar
 */
public class Event{
    private LocalTime aTime;
    private int priority;
    private Consumer<LocalTime> action;

    /**
     * Creates a new event
     * @param aTime activation time
     * @param priority priority
     * @param action action to be performed when this event is processed
     */
    public Event(LocalTime aTime, int priority, Consumer<LocalTime> action) {
        this.aTime = aTime;
        this.priority = priority;
        this.action = action;
    }

    public LocalTime getActivationTime() {
        return aTime;
    }

    public int getPriority() {
        return priority;
    }

    public Consumer<LocalTime> getAction() {
        return action;
    }

    /**
     * Performs the action encapsulated in this event
     * @param t time now
     */
    public void performAction(LocalTime t){
        this.action.accept(t);
    }
}
