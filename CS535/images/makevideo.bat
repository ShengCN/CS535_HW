ffmpeg -i demonstration%d.tiff -s 1080x720 -r 20 -c:v huffyuv -pix_fmt rgb24 out.avi