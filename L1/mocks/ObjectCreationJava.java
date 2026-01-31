public class ObjectCreationJava {
    static class Point {
        int x, y;
        Point(int x, int y) { this.x = x; this.y = y; }
    }

    public static void main(String[] args) {
        final long start = System.nanoTime();
        Point[] arr = new Point[10_000_000];
        for (int i = 0; i < arr.length; i++) {
            arr[i] = new Point(i, i);
        }
        long elapsedMicros = (System.nanoTime() - start) / 1_000;
        System.out.printf("Java: Created %d Points in %d µs\n", arr.length, elapsedMicros);
    }
}

