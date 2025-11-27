from PIL import Image
import struct

SpriteWidth = 16
SpriteHeight = 16

InputFile = "sprites.png"
OutputFile = "sprites.bin"

def main():
    SourceImage = Image.open(InputFile).convert("RGB")
    
    if SourceImage.width % SpriteWidth != 0 or SourceImage.height != SpriteHeight:
        print("Incorrect image format")
        return
        
    SpriteAmount = SourceImage.width // SpriteWidth

    with open(OutputFile, "wb") as FILE:
        for SpriteIndex in range(SpriteAmount):
            OffsetX = SpriteIndex * SpriteWidth
            
            WhitePixels = []
            
            for x in range (SpriteWidth):
                for y in range (SpriteHeight):
                    Pixel = SourceImage.getpixel((OffsetX + x, y))
                    
                    if Pixel[0] > 0 or Pixel[1] > 0 or Pixel[2] > 0:
                        Index = x + y * SpriteWidth
                        WhitePixels.append(Index)
                        
            FILE.write(struct.pack("B", len(WhitePixels)))
            
            for PixelIndex in WhitePixels:
                FILE.write(struct.pack("B", PixelIndex))
                
            WhitePixels.clear()
            
main()