import kotlin.system.exitProcess
import java.io.File
import java.io.IOException
import java.nio.ByteBuffer
import javax.imageio.ImageIO

external fun start()
external fun qrScan(frame: ByteBuffer, width: Int, height: Int, out: ByteBuffer)
external fun end()

fun main(args: Array<String>) {
	//System.loadLibrary("hx3qrdemo")
	if(args.size != 1) {
		println("Expected image path as first and only argument")
		System.exit(1)
	}

	val imgPath = args[0]

	val img = try {
		ImageIO.read(File(imgPath))
	} catch(e: IOException) {
		e.printStackTrace()
		exitProcess(1)
	}

	val width = img.getWidth()
	val height = img.getHeight()

	println("img width: $width, height: $height")

	val pix = img.getRGB(0, 0, width, height, null, 0, width)
	val len = pix.size

	println("pixel array size: $len")

	println("allocating direct bytebuffer")
	val buf = ByteBuffer.allocateDirect(len)

	println("copying pixel data into buffer")
	for(p in pix) {
		buf.put(p.shr(16).and(255).toByte())
		buf.put(p.shr(8).and(255).toByte())
		buf.put(p.and(255).toByte())
	}
}
