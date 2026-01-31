public class MemorySafe {
    public static void main(String[] args) throws InterruptedException {
        for (int i = 0; i < 10000; i++) {
            // Allocate ~1 MB (256k ints * 4 bytes)
            int[] block = new int[256 * 1024];

            // Force-touch memory so OS commits pages
            block[0] = 42;
            block[128] = 99;
            block[block.length - 1] = i;
            Thread.sleep(200);
        }

        System.out.println("Done allocating safely!");
        Thread.sleep(5000); // so you can check with top -pid <PID>
    }

}

