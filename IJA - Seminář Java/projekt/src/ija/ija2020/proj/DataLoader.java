package ija.ija2020.proj;

import ija.ija2020.proj.geometry.Position;
import ija.ija2020.proj.geometry.Rectangle;
import ija.ija2020.proj.store.*;
import ija.ija2020.proj.store.map.StoreMap;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.time.LocalDate;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.opencsv.CSVReader;
import javafx.util.Pair;

/**
 * Reads data from csv files describing a store. See individual methods for descriptions of the corresponding files
 */
public class DataLoader {
    /**
     * Reads a file containing a store map definition. The file should have the following format:
     * ```
     * width, height
     * ```
     * @param filename path to the file to read
     * @return Empty store map
     * @throws FileNotFoundException
     */
    public StoreMap loadMap(String filename) throws IOException {
        CSVReader reader = new CSVReader(new FileReader(filename));

        String[] line = reader.readNext();
        return new StoreMap(Integer.parseInt(line[0]), Integer.parseInt(line[1]));
    }

    /**
     * Loads shelves from a file. The file should have the following format:
     * ```
     * name, x1, y1, x2, y2
     * ...
     * ```
     * @param filename path to the file containing shelves definitions
     * @return List of shelves
     * @throws IOException
     */
    public List<GoodsShelf> loadShelves(String filename) throws IOException {
        CSVReader reader = new CSVReader(new FileReader(filename));

        LinkedList<GoodsShelf> result = new LinkedList<>();

        String[] line;
        while ((line = reader.readNext()) != null){
            String name = line[0];
            int x1 = Integer.parseInt(line[1]);
            int y1 = Integer.parseInt(line[2]);
            int x2 = Integer.parseInt(line[3]);
            int y2 = Integer.parseInt(line[4]);
            result.add(new GoodsShelf(name, new Rectangle(x1, y1, x2, y2)));
        }

        return result;
    }

    /**
     * Loads goodsTypes from a file. The file should have the following format:
     * ```
     * goodsName
     * ...
     * ```
     * @param filename path to the file containing goods names
     * @return List of goods
     * @throws IOException
     */
    public List<Goods> loadGoodsTypes(String filename) throws IOException {
        CSVReader reader = new CSVReader(new FileReader(filename));

        LinkedList<Goods> result = new LinkedList<>();

        String[] line;
        while ((line = reader.readNext()) != null){
            result.add(new Goods(line[0]));
        }

        return result;
    }

    /**
     * Loads orders from a file. The file should have the following format:
     * ```
     * ORDER, priority
     * GoodsType, amount
     * ...
     * ```
     * @param filename path to the file containing orders
     * @param goodsTypes list of permitted goodsTypes
     * @return List of orders
     * @throws IOException
     */
    public List<GoodsOrder> loadOrders(String filename, Map<String, Goods> goodsTypes) throws IOException {
        CSVReader reader = new CSVReader(new FileReader(filename));

        LinkedList<GoodsOrder> result = new LinkedList<>();
        GoodsOrder currentOrder = null;

        String[] line;
        while ((line = reader.readNext()) != null){
            if (line[0].equals("ORDER")){
                if (currentOrder != null){
                    result.add(currentOrder);
                }
                currentOrder = new GoodsOrder(Integer.parseInt(line[1]));
            }else{
                currentOrder.add(new OrderItem(Integer.parseInt(line[1]), goodsTypes.get(line[0])));
            }
        }
        //add last order
        if (currentOrder != null){
            result.add(currentOrder);
        }

        return result;
    }

    public DropOffPoint loadDefaultDropOffPoint(String filename) throws IOException {
        CSVReader reader = new CSVReader(new FileReader(filename));

        String[] line = reader.readNext();
        return new DropOffPoint(new Position(Integer.parseInt(line[0]), Integer.parseInt(line[1])));
    }


    /**
     * Loads default stocks from file. The file should have the following format:
     * ```
     * ShelfName,GoodsType,amount
     * ...
     * ```
     * @param filename
     * @param goodsTypes
     * @return A list of map entries, where the key is the shelf name, and value is the GoodsItem to stock on the given shelf
     * @throws IOException
     */
    public List<Pair<String, GoodsItem>> loadDefaultStocks(String filename, Map<String, Goods> goodsTypes) throws IOException {
        CSVReader reader = new CSVReader(new FileReader(filename));
        List<Pair<String, GoodsItem>> result = new LinkedList<>();

        String[] line;
        while ((line = reader.readNext()) != null) {
            int amount = Integer.parseInt(line[2]);
            for(int i = 0; i < amount; i++){
                result.add(new Pair<String, GoodsItem>(line[0], goodsTypes.get(line[1]).newItem(LocalDate.now())));
            }
        }
        return result;
    }
}
