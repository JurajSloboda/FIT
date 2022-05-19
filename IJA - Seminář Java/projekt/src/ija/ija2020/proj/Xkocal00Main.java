package ija.ija2020.proj;

import ija.ija2020.proj.calendar.Event;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.layout.BorderPane;
import javafx.stage.Stage;

/**
 * Demo class that demonstrates work done by xkocal00 so far
 *
 * Loads data from files in /data folder and initiates the store.
 * Spawns carts to fulfill orders.
 * Movement of the carts can be seen in console.
 */
public class Xkocal00Main extends Application {

    @Override
    public void start(Stage primaryStage) throws Exception{
        BorderPane root = FXMLLoader.load(getClass().getResource("layout.fxml"));
        Scene scene = new Scene(root);
        primaryStage.setTitle("not so usefull bus map");
        primaryStage.setScene(scene);
        primaryStage.show();

        MainController mainController = MainController.getInstance();
        //Start assigning orders
        mainController.getCalendar().insertEvent(new Event(mainController.getTime(), 1, mainController::processOrderAction));


        System.out.println("Starting simulation");
        mainController.startSimulation();
    }


    public static void main(String[] args) {

        launch(args);
    }



}
