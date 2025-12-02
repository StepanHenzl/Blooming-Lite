from PIL import Image
import struct

InputFile = "font.png"
OutputFile = "font.bin"
InputBinding = "FontBinding.txt"

LetterHeight = 5
LetterWidth = 5
LetterSpacing = 1
    

def main():
    SourceImage = Image.open(InputFile).convert("RGB")
    
    if SourceImage.height != LetterHeight:
        print("Incorrect image format")
        return
    
    with open(InputBinding, "r") as FILE:
        FontSymbols = [line.strip() for line in FILE if line.strip()]
        
    LetterAmount = len(FontSymbols)
    
    with open(OutputFile, "wb") as FILE:    
        for LetterIndex in range(LetterAmount):
            OffsetX = LetterIndex * (LetterWidth + LetterSpacing)

            WhitePixels = []
            WidestPixelPosition = 0
            
            for x in range (LetterWidth):
                for y in range (LetterHeight):
                    Pixel = SourceImage.getpixel((OffsetX + x, y))
                    
                    if Pixel[0] > 0 or Pixel[1] > 0 or Pixel[2] > 0:
                        Index = x + y * LetterWidth
                        WhitePixels.append(Index)
                        
                        if x > WidestPixelPosition:
                            WidestPixelPosition = x
                        
            FILE.write(struct.pack("B", len(WhitePixels) + 2))
            
            FILE.write(struct.pack("B", ord(FontSymbols[LetterIndex])))
            
            FILE.write(struct.pack("B", WidestPixelPosition + 1))
            
            for PixelIndex in WhitePixels:
                FILE.write(struct.pack("B", PixelIndex))
            
        
main()