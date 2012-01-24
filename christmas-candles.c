const unsigned int candle_width=10;
const char
	rising_color_name[]="green",
	falling_color_name[]="red",
	font_name[]="*-helvetica-medium-r-normal*--8-*";
const float volume_pane_fraction=1.0/3;

#include <X11/Xlib.h>
#include <string.h>
#include <unistd.h>
#include <die.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

struct candle {
	unsigned int year,month,day;
	float open,high,low,close;
	unsigned long volume; };

unsigned int max(unsigned int a, unsigned int b){ return (a>b)?a:b; }

int main(){
	Display * display;
	Window window, root_window;
	GC background_gc,labels_gc,rising_gc,falling_gc,current_gc;
	int screen,x=0,y=0;
	unsigned int i,width,height,border_width,depth,n_candles=0,n_alloc=0,candle_width,year,month,day;
	float open,high,low,close,high_print=0,low_print=0;
	unsigned long volume, max_volume=0;
	XGCValues background_gc_values, labels_gc_values, rising_gc_values, falling_gc_values;
	Visual * visual;
	Colormap colormap;
	XColor rising_color_on_screen, rising_color_rgb, falling_color_on_screen, falling_color_rgb;
	XFontStruct * font_info;
	XEvent event;
	struct candle * candles=NULL;
	char buf[81];

	while(!feof(stdin)){
		if(fscanf(stdin,"%u-%u-%u,%f,%f,%f,%f,%lu\n",&year,&month,&day,&open,&high,&low,&close,&volume)!=8) break;
		if(n_candles==n_alloc){
			n_alloc=max(1,n_alloc*exp(1));
			candles=realloc(candles,n_alloc*sizeof(struct candle)); }
		candles[n_candles].year=year;
		candles[n_candles].month=month;
		candles[n_candles].day=day;
		candles[n_candles].volume=volume;
		candles[n_candles].open=open;
		candles[n_candles].high=high;
		candles[n_candles].low=low;
		candles[n_candles++].close=close;
		if(volume>max_volume) max_volume=volume;
		if(low_print==0) low_print=low;
		if(low<low_print) low_print=low;
		if(high>high_print) high_print=high; }

	if((display=XOpenDisplay(NULL))==NULL) DIE;
	screen=XDefaultScreen(display);
	width=DisplayWidth(display,screen)/3;
	height=DisplayHeight(display,screen)/3;
	window=XCreateSimpleWindow(
		display,
		RootWindow(display,screen),
		x,y,
		width,height,
		0, BlackPixel(display,screen),
		BlackPixel(display,screen));
	XSelectInput(display,window,ExposureMask|PointerMotionMask);
	XMapWindow(display,window);

	background_gc = XCreateGC(display,window,0,&background_gc_values);
	XSetForeground(display,background_gc,BlackPixel(display,screen));
	XSetBackground(display,background_gc,BlackPixel(display,screen));
	XSetFillStyle(display,background_gc,FillSolid);

	labels_gc = XCreateGC(display,window,0,&labels_gc_values);
	XSetForeground(display,labels_gc,WhitePixel(display,screen));
	XSetBackground(display,labels_gc,BlackPixel(display,screen));
	font_info=XLoadQueryFont(display,font_name);
	if(!font_info) DIE;
	XSetFont(display,labels_gc,font_info->fid);

	visual = DefaultVisual(display,screen);
	colormap = XCreateColormap(display,window,visual,AllocNone);
	XAllocNamedColor(display,colormap,rising_color_name,&rising_color_on_screen,&rising_color_rgb);
	XAllocNamedColor(display,colormap,falling_color_name,&falling_color_on_screen,&falling_color_rgb);

	rising_gc = XCreateGC(display,window,0,&rising_gc_values);
	XSetForeground(display,rising_gc,rising_color_on_screen.pixel);
	XSetBackground(display,rising_gc,BlackPixel(display,screen));
	XSetLineAttributes(display,rising_gc,1,LineSolid,CapButt,JoinBevel);

	falling_gc = XCreateGC(display,window,0,&falling_gc_values);
	XSetForeground(display,falling_gc,falling_color_on_screen.pixel);
	XSetBackground(display,falling_gc,BlackPixel(display,screen));
	XSetLineAttributes(display,falling_gc,1,LineSolid,CapButt,JoinBevel);

	while(1){
		XNextEvent(display,&event);
		switch(event.type){
			case Expose:
				if(event.xexpose.count>0) break;
				XGetGeometry(display,window,&root_window,&x,&y,&width,&height,&border_width,&depth);
				candle_width=width/n_candles;
				if(candle_width<3) candle_width=3;
				for(i=0;i<n_candles;i++){
					x=(n_candles-1-i)*width/n_candles;
					if
						(candles[i].open<=candles[i].close)
						{
							current_gc=rising_gc;
							XDrawRectangle(
								display,
								window,
								current_gc,
								x+1
								,
									height
									*(1-volume_pane_fraction)
									*(1-
										(candles[i].close-low_print)
										/(high_print-low_print))
								,candle_width-2
								,
									height
									*(1-volume_pane_fraction)
									*(candles[i].close-candles[i].open)
									/(high_print-low_print));
						} else {
							current_gc=falling_gc;	
							XDrawRectangle(
								display,
								window,
								current_gc,
								x+1
								,
									height
									*(1-volume_pane_fraction)
									*(1-
										(candles[i].open-low_print)
										/(high_print-low_print))
								,candle_width-2
								,
									height
									*(1-volume_pane_fraction)
									*(candles[i].open-candles[i].close)
									/(high_print-low_print)); }
					XDrawLine(
						display,
						window,
						current_gc,
						x+candle_width/2
						,
							height
							*(1-volume_pane_fraction)
							*(1-
								(candles[i].high-low_print)
								/(high_print-low_print)),
						x+candle_width/2
						,
							height
							*(1-volume_pane_fraction)
							*(1-
								(candles[i].low-low_print)
								/(high_print-low_print)));
					XFillRectangle(
						display
						,window
						,current_gc
						,x+1
						,
							height*(1-volume_pane_fraction)
							+
								height*volume_pane_fraction
								*
									(max_volume-candles[i].volume)
									/(float)max_volume
						,candle_width-2
						,
							height
							*volume_pane_fraction
							*(candles[i].volume/(float)max_volume)); }
				/* for(i=0;i<height;i+=2*(font_info->ascent+font_info->descent)){
					sprintf(buf,"- %5.5g",low_print+(high_print-low_print)*(height-i)/height);
					XDrawString(display,window,labels_gc,width-35,i,buf,strlen(buf)); }*/
				XFlush(display);
				XSync(display,False);
				break;
			case MotionNotify:
				i=n_candles*(width-event.xmotion.x)/width;
				if(i==n_candles) i=n_candles-1;
				if
					(event.xmotion.y<=height*(1-volume_pane_fraction))
					sprintf(buf,"%u %.2u %.2u  /  %.5g  "
						,candles[i].year
						,candles[i].month
						,candles[i].day
						,low_print+(high_print-low_print)*(height*(1-volume_pane_fraction)-event.xmotion.y)/(height*(1-volume_pane_fraction)));
					else sprintf(buf,"%u %.2u %.2u  /  %*lu  "
						,candles[i].year
						,candles[i].month
						,candles[i].day
						,(int)(log(max_volume)/log(10)+1)
						,candles[i].volume);
				XFillRectangle(display,window,background_gc,0,height-3*(font_info->ascent+font_info->descent),XTextWidth(font_info,buf,strlen(buf)),font_info->ascent+font_info->descent);
				XDrawString(display,window,labels_gc,0,height-2*(font_info->ascent+font_info->descent),buf,strlen(buf));
				XFlush(display);
				XSync(display,False); }} 
	XCloseDisplay(display);
	return 0; }
