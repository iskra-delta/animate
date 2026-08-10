#include <stdint.h>
#include <stdio.h>
#include <ugpx.h>
#include <partner/conio.h>

#include <animate.h>

static uint8_t gnextpage[] = { 1, 0 };

static void gclsex(uint8_t endpage) {
    gsetpage(PG_DISPLAY,gnextpage[endpage]);
    gcls();
    gsetpage(PG_DISPLAY,endpage);
    gcls();
}

void animate(animation_t *ani) {

    /* calculate screen mid */
    int x=SCREEN_WIDTH/2-ani->width/2,
        y=SCREEN_HEIGHT/2-ani->height/2;

    /* initialize graphics. */
    ginit(RES_1024x256);

    /* clear screen and switch to page 1 */
    gclsex(1);

    /* draw single frame for debugging */
    uint8_t *frames=(uint8_t *)&(ani->offsets[ani->frames]);

    /* and main loop */
    uint8_t page=0;
    gsetpage(PG_WRITE,page);
    int prev=-1;
    int frame=0;

    /* and loop */
    while (!kbhit()) {

        /* get the correct sprite. */
        int curr=frame++; if (frame==ani->frames) frame=0;
        /* set write page and draw sprite */
        gsetcolor(CO_FORE);
        gputglyph(frames+ani->offsets[curr],x,y);
        gsetpage(PG_DISPLAY,page);
        /* switch page */
        page=gnextpage[page];

        /* and delete previous */
        if (prev >= 0) {
            gsetpage(PG_WRITE,page);
            gsetcolor(CO_BACK);
            gputglyph(frames+ani->offsets[prev],x,y);
        } else 
            gsetpage(PG_WRITE,page);
        
        /* and set previous */
        prev=curr;
    };   

    /* clear screen and switch to page 0 */
    gclsex(0);

    /* exit graphics. */
    gexit();
}