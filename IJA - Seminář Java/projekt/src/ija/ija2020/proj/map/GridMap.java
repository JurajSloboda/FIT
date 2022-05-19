package ija.ija2020.proj.map;

import ija.ija2020.proj.MainController;
import ija.ija2020.proj.geometry.Targetable;

import java.util.LinkedList;
import java.util.List;

public class GridMap {
    private final int width;
    private final int height;
    protected final GridNode[] grid;

    /**
     * New grid map with hte specified dimensions
     * @param height
     * @param width
     */
    public GridMap(int height, int width) {
        this.width = width;
        this.height = height;
        this.grid = new GridNode[width*height];
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                grid[i + j * width] = new GridNode(i, j, this);
            }
        }
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public int getSize(){ return width * height; }

    /**
     * Get node by coordinates
     * @param x horizontal (x) coordinate of the node
     * @param y vertical (y) coordinate of the node
     * @return The node on this map with the given coordinates
     */
    public GridNode getNode(int x, int y){
        int index = x + y * width;
        if (index >= width * height || index < 0) {
            return null;
        }else{
            return grid[index];
        }
    }

    /**
     * Get node by position
     * @param pos Position of the node
     * @return node on this map that has the same coordinates as the specified position, or null if there is no such node
     */
    public GridNode getNode(Targetable pos){
        int index = pos.getX() + pos.getY() * width;
        if (index >= width * height || index < 0) {
            return null;
        }else{
            return grid[index];
        }
    }

//    private final int[][] NEIGHBOUR_MODS = {{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1}};
    private final int[][] NEIGHBOUR_MODS = {{1,0},{0,-1},{-1,0},{0,1}};

    /**
     * Get a list of nodes adjacent to the specified position that are not obstructed
     * @param x vertical coordinate of the central node
     * @param y horizontal coordinate of the central node
     * @return all unobstructed nodes adjacent to the specified central node
     */
    public List<GridNode> getUnobstructedNeighbours(int x, int y){
        List<GridNode> result = new LinkedList<GridNode>();
        //System.out.println(String.format("+Node(%d,%d), neigh:{"));
        for(int[] mod : NEIGHBOUR_MODS){
            int x1 = x + mod[0];
            int y1 = y + mod[1];
            if(x1 < 0 || y1 < 0 || x1 > this.width || y1 > this.height){
                continue;
            }
            GridNode neigh = getNode(x1, y1);
            if (neigh != null && !neigh.isObstructed()){
                result.add(neigh);
            }
        }

        return result;
    }

    /**
     * Get a list of nodes adjacent to the specified node that are not obstructed
     * @param node central node
     * @return all unobstructed nodes adjacent to the specified central node
     */
    public List<GridNode> getUnobstructedNeighbours(GridNode node){
        List<GridNode> result = new LinkedList<GridNode>();

        for(int[] mod : NEIGHBOUR_MODS){
            int y = node.getY();
            int x = node.getX();
            GridNode neigh = getNode(x +mod[0], y +mod[1]);
            if (neigh != null && !neigh.isObstructed()){
                result.add(neigh);
            }
        }

        return result;
    }
}
