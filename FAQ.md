# Fun SDK's FAQ

##Index
[Common errors](#common-errors)

---

## Common Errors
> In Windows, why do I get the error '*DirectShowPlayerService::doSetUrlSource: Unresolved error code 0x80004005*' when I try to open a MP4 video?

Because the default directshow filters on Windows [are not enough](http://stackoverflow.com/a/32436805/2896619) for the playback of video files with that container format. Usually installing a codec package like the [K-Lite Codec Pack](http://www.codecguide.com/) will help.

---