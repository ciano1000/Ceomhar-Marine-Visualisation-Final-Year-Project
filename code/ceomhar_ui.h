struct ScreenPos 
{
	f32 x;
    f32 y;
};

ScreenPos PixelsToDIP(ScreenPos pixel_pos);
ScreenPos DIPToPixels(ScreenPos dpi_pos);