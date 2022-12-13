#pragma once

class drawgon
{
public:
    // constructor/destructor:
    drawgon(void);
    ~drawgon(void);

    //members:
    int drawCtrlIdx,    // 0=nothing, 1=polygon, 2=dots, 3=dots&polygon, 4=dots&animation, 5=smiley, 6=number, 7=clrscr, 8=experiment
        drawNumber,     // 0=nothing, 1-12=draw
        drawColorIndex, // 0-12: black, red, green, blue, violet, $green, sky-blue, dark-pink, light-purple, yellow, lime, orange, white
        bsegCount,      // bezier segments in vFrame total points = 3 * bsegCount + 1
        lsegCount,      // line segment count in vFrame total points = 2*lsegCount
        sCount,         // Voronoi site points in vFrame total points = sCount
        emojiIdx,
        startDrawAt;
    NGons * png;
    Gtk::DrawingArea * draw_win;
    std::vector<CompGeo::XY> * dots, 
         * vFrame;      // here's the idea:  vFrame will hold in order [3*bsegCount+1 points][2*lsegCount points][sCount points]
                        // the first line segment points will be the directrix, the others will be Voronoi edges
                        // the sCount will be site points above or on the directrix in added order
    //std::vector<int> V_BMP_IDX;  // a lookup index for each Voronoi vertex point to match with area of emoji.png
    const Cairo::RefPtr< ::Cairo::Context> * pCR;
    Gtk::Allocation work_area;
    CompGeo::XY ctr;
    static const Cairo::RefPtr<Cairo::SolidPattern> cvClr[]; //these patterns correspond to colors as in drawColorIndex above
    //Cairo::RefPtr<Cairo::ImageSurface> image;   // holds emoji.png
    //Cairo::ImageSurface::SlotReadFunc my_slot;
    //unsigned char * RdPos;
    //sigc::nil vBuff;
    //Cairo::Surface::SlotDestroy dummy;
    //sigc::slot<void, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil, sigc::nil> my_slot;
    //methods:
    void switchboard(const Cairo::RefPtr< ::Cairo::Context>&);
    void draw_smiley(void);
    void draw_number(void);
    void draw_polygon(void);
    void draw_dots(void);
    void draw_dotsandpolygon(void);
    void draw_dotsandanimation(void);
    void clrscr(void);
    //void experimental(void);
    //Cairo::ErrorStatus my_read_func(unsigned char*, unsigned int);
    //void on_destroy(void);
};
