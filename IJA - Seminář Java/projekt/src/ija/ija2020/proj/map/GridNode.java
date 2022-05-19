package ija.ija2020.proj.map;

import ija.ija2020.proj.geometry.Drawable;
import ija.ija2020.proj.geometry.Targetable;
import javafx.scene.shape.Shape;

import java.util.*;
import java.util.List;

public class GridNode extends Observable implements Targetable, Drawable {
    private final int x;
    private final int y;
    protected boolean obstructed;
    private final GridMap parentMap;

    public GridNode(int x, int y, GridMap parentMap) {
        this.x = x;
        this.y = y;
        this.parentMap = parentMap;
        this.obstructed = false;
    }

    public GridNode(int x, int y, GridMap parentMap, boolean obstructed) {
        this.x = x;
        this.y = y;
        this.parentMap = parentMap;
        this.obstructed = obstructed;
    }

    public boolean isObstructed() {
        return obstructed;
    }

    @Override
    public int getX() {
        return x;
    }

    @Override
    public int getY() {
        return y;
    }

    @Override
    public int distance(Targetable target) {
        //Manhattan Distance
        return Math.abs(target.getX() - this.x) + Math.abs(target.getY() - this.y);
    }

    public void setObstructed(boolean obstructed) {
        this.obstructed = obstructed;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        GridNode gridNode = (GridNode) o;
        return x == gridNode.x && y == gridNode.y;
    }

    @Override
    public int hashCode() {
        return Objects.hash(x, y);
    }

    /**
     * Finds a path from this node to another node through a grid.
     * Only passes unobstructed nodes.
     * Uses A* with euqlidean distance as heuristic to find the path
     * @param grid
     * @param end
     * @return Path from the END node to the START node (needs to be polled from the tail)
     */
    public Deque<GridNode> getPathToNodeOnGrid(GridMap grid, GridNode end){
        class MazeNode{
            public int x;
            public int y;

            public MazeNode(int x, int y) {
                this.x = x;
                this.y = y;
            }

            @Override
            public boolean equals(Object o) {
                if (this == o) return true;
                if (o == null || getClass() != o.getClass()) return false;
                MazeNode mazeNode = (MazeNode) o;
                return x == mazeNode.x && y == mazeNode.y;
            }

            @Override
            public int hashCode() {
                return Objects.hash(x, y);
            }

            public int f = Integer.MAX_VALUE;
            public int g = Integer.MAX_VALUE; //cost from start
            public int h = Integer.MAX_VALUE; //manhatan distance to end
            public MazeNode parent = null;
            public boolean visited = false;
            public boolean hComputed = false;
        }

        class MazeNodeComparator implements Comparator<MazeNode> {
            @Override
            public int compare(MazeNode o1, MazeNode o2) {
                if(o1.f < o2.f){
                    return -1;
                }else if(o1.f > o2.f){
                    return 1;
                }
                return 0;
            }
        }

        //A* with manhatan distance
        MazeNode endMazeNode = null;

        //prepare the maze
        int width = grid.getWidth();
        int height = grid.getHeight();
        MazeNode[] maze = new MazeNode[width*height];
        //prepare maze queue
        PriorityQueue<MazeNode> open = new PriorityQueue<MazeNode>(new MazeNodeComparator());

        //prepare start node
        MazeNode start = new MazeNode(this.x, this.y);
        start.f = 0;
        start.g = 0;
        start.h = Math.abs(end.getX() - start.x) + Math.abs(end.getY() - start.y);
        start.hComputed = true;
        open.add(start);

        while (!open.isEmpty()){
            //get first node from open
            MazeNode node = open.poll();
            node.visited = true;

            //generate neighbours
            List<GridNode> neighbours = grid.getUnobstructedNeighbours(node.x, node.y);
            for(GridNode neigh : neighbours){
                MazeNode mazeNeigh = maze[neigh.x + neigh.y * width];
                if (mazeNeigh == null){
                    mazeNeigh = new MazeNode(neigh.x, neigh.y);
                    maze[neigh.x + neigh.y * width] = mazeNeigh;
                }
                if (mazeNeigh.visited){
                    continue;
                }
                //if (neigh == end) {
                if (neigh.x == end.getX() && neigh.y == end.getY()){
                    mazeNeigh.parent = node;
                    endMazeNode = mazeNeigh;
                    break;
                }

                //update root distance
                mazeNeigh.g = Math.min(mazeNeigh.g, node.g+1);

                //update heuristic
                if(!mazeNeigh.hComputed) {
                    mazeNeigh.h = Math.abs(end.getX() - mazeNeigh.x) + Math.abs(end.getY() - mazeNeigh.y);
                    mazeNeigh.hComputed = true;
                }

                //compute score
                final int minDistance = Math.min(mazeNeigh.f, mazeNeigh.g+mazeNeigh.h);
                if (minDistance != mazeNeigh.f){
                    mazeNeigh.f = minDistance;
                    mazeNeigh.parent = node;
                }

                if (!open.contains(mazeNeigh)) {
                    open.add(mazeNeigh);
                }
            }

            if(endMazeNode != null){
                break;
            }
        }

        //get the route (or failure)
        if(endMazeNode != null){
            Deque<GridNode> result = new LinkedList<GridNode>();
            MazeNode node = endMazeNode;
            while(node.parent != null){
                result.add(grid.getNode(node.x, node.y));
                node = node.parent;
            }
            return result;
        }else{
            return null;
        }
    }

    /**
     * Finds the node that is closest to the target, and is adjacent to this node and is unobstructed
     * @param target
     * @return the closest unobstructed node to target that is adjacent to this, or null if no such node exists (i.e. if all adjacent nodes are obstructed)
     */
    public GridNode getClosestUnobstructedAdjacent(Targetable target) {
        List<GridNode> neigh = this.parentMap.getUnobstructedNeighbours(this);
        GridNode result = null;
        int distance = Integer.MAX_VALUE;

        for(GridNode node : neigh){
            int newDistance = node.distance(target);
            if(newDistance < distance){
                result = node;
                distance = newDistance;
            }
        }
        return result;
    }

    @Override
    public Map<String, Integer>  getContent() {
        return null;
    }

    @Override
    public boolean isHovered(){
        return false;
    }
    @Override
    public List<Shape> getGUI() {

        return null;
    }
}
