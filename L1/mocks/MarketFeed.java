import java.io.*;
import java.net.Socket;

class MarketData {
    long timestamp;
    double price;
    int volume;
}

public class MarketFeed {
    public static void main(String[] args) throws Exception {
        Socket socket = new Socket("localhost", 5555);
        DataInputStream dis = new DataInputStream(socket.getInputStream());
        byte[] buffer = new byte[20];

        long start = System.nanoTime();

        for (int i = 0; i < 1_000_000; i++) {
            dis.readFully(buffer);
            MarketData md = parse(buffer); // creates new object each time
            // Decision logic
        }

        long end = System.nanoTime();
        System.out.println("Elapsed: " + (end - start)/1000 + " us");
        socket.close();
    }

    private static MarketData parse(byte[] buffer) throws IOException {
        DataInputStream dis = new DataInputStream(new ByteArrayInputStream(buffer));
        MarketData md = new MarketData();
        md.timestamp = dis.readLong();
        md.price = dis.readDouble();
        md.volume = dis.readInt();
        return md;
    }
}

