package ija.ija2020.proj.calendar;

import java.time.LocalTime;
import java.util.Comparator;
import java.util.PriorityQueue;

/**
 * Calendar for discreet simulations of NextEvent type.
 * @see Event
 */
public class Calendar {

    PriorityQueue<Event> queue;

    public Calendar() {
        this.queue = new PriorityQueue<Event>(new EventComparator());
    }

    class EventComparator implements Comparator<Event> {
        @Override
        public int compare(Event o1, Event o2) {
            //NOTE: head of a priority queue is the least element
            if(o1.getActivationTime().isBefore(o2.getActivationTime())){
                return -1;
            }else if(o1.getActivationTime().isAfter(o2.getActivationTime())){
                return 1;
            }else{ //identical activation time
                if(o1.getPriority() < o2.getPriority()){
                    return 1;
                }else if (o1.getPriority() > o2.getPriority()){
                    return -1;
                }else{
                    return 0;
                }
            }
        }
    }

    /**
     * Removes the next event from this calendar.
     * @return Next event to be processed, or null if the calendar is empty
     */
    public Event getNextEvent(){
        return queue.poll();
    }

    /**
     * Queues another event to the calendar
     * @param e Event to be queued
     */
    public void insertEvent(Event e){
        queue.add(e);
    }
}
