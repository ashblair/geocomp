#include "first.hpp"
#include "CompGeo.hpp"
#include "NGons.hpp"
#include "drawgon.hpp"

using namespace std;

extern unsigned char emoji_start    asm("_binary_rsc_emoji_png_start");
extern unsigned char emoji_end      asm("_binary_rsc_emoji_png_end");
//extern unsigned char _binary_emoji_png_start, _binary_emoji_png_end;

const Cairo::RefPtr<Cairo::SolidPattern> drawgon::cvClr[] ={
        Cairo::SolidPattern::create_rgba(0, 0, 0, 0.5),           //0 black
        Cairo::SolidPattern::create_rgba(1, 0, 0, 1),           //1 red
        Cairo::SolidPattern::create_rgba(0, 1, 0, 1),           //2 green
        Cairo::SolidPattern::create_rgba(0, 0, 1, 1),           //3 blue
        Cairo::SolidPattern::create_rgba(0.5, 0.5, 1, 1),       //4 violet
        Cairo::SolidPattern::create_rgba(0.5, 0.75, 0.5, 1),    //5 $green$
        Cairo::SolidPattern::create_rgba(0.5, 1, 1, 1),         //6 sky-blue
        Cairo::SolidPattern::create_rgba(1, 0.5, 0.5, 1),       //7 dark-pink
        Cairo::SolidPattern::create_rgba(1, 0.75, 1, 1),        //8 light-purple
        Cairo::SolidPattern::create_rgba(1, 1, 0.5, 1),         //9 yellow
        Cairo::SolidPattern::create_rgba(0.75, 1, 0, 1),        //10 lime
        Cairo::SolidPattern::create_rgba(1, 0.5, 0, 1),         //11 orange
        Cairo::SolidPattern::create_rgba(1, 1, 1, 1),            //12 white
        Cairo::SolidPattern::create_rgba(0.1, 0.1, 0.1, 0.05)        //13 light gray fill
    };

drawgon::drawgon(void)
{
    drawCtrlIdx = 0;    // 0=nothing, 1=polygon, 2=dots, 3=dots&polygon, 4=dots&animation, 5=smiley, 6=number
    drawNumber = 0;
    drawColorIndex = 0;
    bsegCount = 0;
    lsegCount = 0;
    sCount = 0;
    emojiIdx = 0;
    startDrawAt = 0;
    png = NULL;
    draw_win = NULL;
    dots= NULL;
    vFrame = NULL;
    pCR = NULL;
    work_area = Gtk::Allocation(0, 0, 0, 0);
    ctr = CompGeo::XY(0.0, 0.0);
    //image = Cairo::ImageSurface::create_from_png("resources/emoji.png");
    
    unsigned char * i_s = &emoji_start; //, * i_e = &_binary_emoji_png_end;
    //unsigned int i_z = i_e - i_s;
    //unsigned char * cBuff = (unsigned char *) new unsigned char[i_z];
    RdPos = i_s;
    

    //my_slot = sigc::bind(sigc::mem_fun(*this, &drawgon::my_read_func), cBuff, i_z);
    my_slot = sigc::mem_fun<unsigned char*, unsigned int, Cairo::ErrorStatus>(*this, &drawgon::my_read_func);
    image = Cairo::ImageSurface::create_from_png_stream(my_slot);
    //image->create_from_png_stream(my_slot);
    //delete [] cBuff;
    //my_slot = sigc::mem_fun(*this, &drawgon::on_destroy);
    //string mimeType = "CAIRO_MIME_TYPE_PNG";
    //image->set_mime_data(mimeType, i_s, i_z, my_slot);
    
    //image->create_from_png("resources/emoji.png");
    //image->create_from_png_stream()
    //cout << "constructing drawgon \n";

}

drawgon::~drawgon(void)
{

}
/*
void drawgon::on_destroy(void)
{ // as this image is embedded in the app file no deletion is needed

}
*/
Cairo::ErrorStatus drawgon::my_read_func(unsigned char* data, unsigned int length)
{
    unsigned char * i_e = &emoji_end;
    if (RdPos >= i_e) return CAIRO_STATUS_READ_ERROR;
    unsigned int i_z = i_e - RdPos, rmng = i_z < length? i_z: length;
    memcpy(data, RdPos, rmng);
    RdPos += rmng;
    return CAIRO_STATUS_SUCCESS;
} 

void drawgon::switchboard(const Cairo::RefPtr< ::Cairo::Context> & cr)
{
    if ((drawCtrlIdx == 0) || (drawCtrlIdx > 8)) return;
    pCR = &cr;
    work_area = draw_win->get_allocation();
    ctr.x = work_area.get_width() / 2.0;
    ctr.y = work_area.get_height() / 2.0;

    switch(drawCtrlIdx)
    {
    case 1:
        draw_polygon();
        break;
    case 2:
        draw_dots();
        break;
    case 3:
        draw_dotsandpolygon();
        break;
    case 4:
        draw_dotsandanimation();
        break;
    case 5:
        draw_smiley();
        break;
    case 6:
        draw_number();
        break;
    case 7:
        clrscr();
        break;
    case 8:
        experimental();
        break;
    }

}

void drawgon::draw_smiley(void)
{
    const Cairo::RefPtr<Cairo::Context>& cr = *pCR;
    // THIS IS SMILEY:
    int width = work_area.get_width(), height = work_area.get_height(),
        min = width < height?width:height;
    double cx = ctr.x, cy = ctr.y, radius = min / 2.0 - 10.0,
        side = 2.0 * radius, half = side / 2.0, minx = -half, miny = -half, 
        maxx = half, maxy = half;

    //cout << "allocated width: " << to_string(width) << "\n";
    //cout << "allocated height: " << to_string(height) << "\n";

    //cr->scale(1.0, 1.0);
    cr->translate(cx, cy);


    Cairo::RefPtr<Cairo::LinearGradient>plg = (Cairo::LinearGradient::create(0.0, 0.0,  0.0, 256.0));
    plg->add_color_stop_rgba(1, 0.5, 0.5, 0.5, 1);
    plg->add_color_stop_rgba(0, 1, 1, 1, 1);

    //cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 1);
    //cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 1);
    // was 256
    cr->rectangle (-cx, -cy, width, height);


    cr->set_source (plg);

    cr->fill ();
    //Cairo::RefPtr<Cairo::RadialGradient>prg = (Cairo::RadialGradient::create(115.2, 102.4, 25.6, 102.4,  102.4, 128.0));
    double ceff = radius / 76.8;
    Cairo::RefPtr<Cairo::RadialGradient>prg = (Cairo::RadialGradient::create(-13 * ceff, -26 * ceff, radius / 3.0, -26 * ceff,  -26 * ceff, 5 * radius / 3));
    //pat = cairo_pattern_create_radial (115.2, 102.4, 25.6, 102.4,  102.4, 128.0);
    prg->add_color_stop_rgba (0, 1, 1, 0, 1); // yellow
    prg->add_color_stop_rgba (1, 0.3125, 0.3125, 0, 1); // brown
    cr->set_source (prg);
    //cr->arc (128.0, 128.0, 76.8, 0, 2 * M_PI);
    cr->arc (0, 0, radius, 0, 2 * M_PI);
    cr->fill ();

    // now back to black somehow:
    Cairo::RefPtr<Cairo::SolidPattern>spat = Cairo::SolidPattern::create_rgb(0.0, 0.0, 0.0);
    cr->set_source(spat);
    double r_e = radius / 6.0, r_s = 5 * r_e, xFact = 5 * r_e / 2;
    cr->arc(xFact, -r_e, r_e, 0, 2 * M_PI);
    cr->arc(-xFact, -r_e, r_e, 0, 2 * M_PI);
    cr->fill();
    cr->arc(0, 0, 5 * r_e, 0, M_PI);
    cr->set_line_width(min * 0.0152); 
    cr->stroke();


}

void drawgon::draw_number(void)
{
    assert ((drawNumber > 0) && (drawNumber <= 12));
	// Variable declarations 
	// Color data:
    /*
    Cairo::RefPtr<Cairo::SolidPattern> cvClr[] ={
        Cairo::SolidPattern::create_rgba(1, 0, 0, 1), 
        Cairo::SolidPattern::create_rgba(0, 1, 0, 1), 
        Cairo::SolidPattern::create_rgba(0, 0, 1, 1), 
        Cairo::SolidPattern::create_rgba(0.5, 0.5, 1, 1), 
        Cairo::SolidPattern::create_rgba(0.5, 0.75, 0.5, 1), 
        Cairo::SolidPattern::create_rgba(0.5, 1, 1, 1), 
        Cairo::SolidPattern::create_rgba(1, 0.5, 0.5, 1), 
        Cairo::SolidPattern::create_rgba(1, 0.75, 1, 1), 
        Cairo::SolidPattern::create_rgba(1, 1, 0.5, 1), 
        Cairo::SolidPattern::create_rgba(0.75, 1, 0, 1),
        Cairo::SolidPattern::create_rgba(1, 0.5, 0, 1), 
        Cairo::SolidPattern::create_rgba(1, 1, 1, 1)
    };
    */
	int numClrs = sizeof(cvClr) / sizeof(cvClr[0]);

    const Cairo::RefPtr<Cairo::Context>& cr = *pCR;
    int width = work_area.get_width(), height = work_area.get_height(),
        min = width < height?width:height;
    double cx = ctr.x, cy = ctr.y;
    //static Cairo::RefPtr<Cairo::ImageSurface> surface = 
    //    Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);

	double x, y, px, ux=1, uy=1, dashlength;
    string txt = to_string(drawNumber);

    Cairo::FontExtents f_e;
    Cairo::TextExtents t_e;


    //Cairo::RefPtr<Cairo::ImageSurface> surface =
    //Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
    
    //cr->create(surface);
    cr->set_source(cvClr[drawNumber]);
    cr->paint();
    //cr->set_font_size(width / 1.2);
    Cairo::Matrix cm(width / 1.2, 0, 0, height, 0, 0);
    cr->set_font_matrix(cm);
	cr->set_source_rgb(0.0, 0.0, 0.0); // black printing
	cr->select_font_face("Symbol", 
        Cairo::FontSlant::FONT_SLANT_NORMAL, 
        Cairo::FontWeight::FONT_WEIGHT_BOLD);
	cr->get_font_extents(f_e);
	cr->device_to_user_distance(ux, uy);
	if (ux > uy)
		px = ux;
	else
		px = uy;
	cr->get_font_extents(f_e);
    cr->get_text_extents(txt, t_e);
	x = cx - t_e.x_bearing - t_e.width / 2;
	y = cy - f_e.descent + f_e.height / 2;
	cr->move_to(x, y);
	//cairo_set_source_rgb (cr, 0, 0, 0);
	cr->show_text(txt);

	// Prepare drawing area 
	//surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 600, 600);
	//cr = cairo_create (surface);
	// Example is in 26.0 x 1.0 coordinate space 
	//cairo_scale (cr, 600, 600);
	//cairo_set_font_size (cr, 0.5);
    
	// Drawing code goes here 
	/* baseline, descent, ascent, height 
	cairo_set_line_width (cr, 4*px);
	dashlength = 9*px;
	cairo_set_dash (cr, &dashlength, 1, 0);
	cairo_set_source_rgba (cr, 0, 0.6, 0, 0.5);
	cairo_move_to (cr, x + te.x_bearing, y);
	cairo_rel_line_to (cr, te.width, 0);
	cairo_move_to (cr, x + te.x_bearing, y + fe.descent);
	cairo_rel_line_to (cr, te.width, 0);
	cairo_move_to (cr, x + te.x_bearing, y - fe.ascent);
	cairo_rel_line_to (cr, te.width, 0);
	cairo_move_to (cr, x + te.x_bearing, y - fe.height);
	cairo_rel_line_to (cr, te.width, 0);
	cairo_stroke (cr);

	/* extents: width & height 
	cairo_set_source_rgba (cr, 0, 0, 0.75, 0.5);
	cairo_set_line_width (cr, px);
	dashlength = 3*px;
	cairo_set_dash (cr, &dashlength, 1, 0);
	cairo_rectangle (cr, x + te.x_bearing, y + te.y_bearing, te.width, te.height);
	cairo_stroke (cr);

	/* text 
	cairo_move_to (cr, x, y);
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_show_text (cr, text);

	/* bearing 
	cairo_set_dash (cr, NULL, 0, 0);
	cairo_set_line_width (cr, 2 * px);
	cairo_set_source_rgba (cr, 0, 0, 0.75, 0.5);
	cairo_move_to (cr, x, y);
	cairo_rel_line_to (cr, te.x_bearing, te.y_bearing);
	cairo_stroke (cr);

	/* text's advance 
	cairo_set_source_rgba (cr, 0, 0, 0.75, 0.5);
	cairo_arc (cr, x + te.x_advance, y + te.y_advance, 5 * px, 0, 2 * M_PI);
	cairo_fill (cr);

	/* reference point 
	cairo_arc (cr, x, y, 5 * px, 0, 2 * M_PI);
	cairo_set_source_rgba (cr, 0.75, 0, 0, 0.5);
	cairo_fill (cr);

	/* Write output and clean up 
	cairo_surface_write_to_png (surface, "textextents.png");
	cairo_destroy (cr);
	cairo_surface_destroy (surface);
	*/

}
void drawgon::clrscr(void)
{
    const Cairo::RefPtr<Cairo::Context>& cr = *pCR;
    cr->set_source_rgb(1.0, 1.0, 1.0); // white
    cr->paint();  // clear screen

}

void drawgon::draw_polygon(void)
{

    const Cairo::RefPtr<Cairo::Context>& cr = *pCR;
    int width = work_area.get_width(), height = work_area.get_height(),
        min = width < height?width:height;
    double cx = ctr.x, cy = ctr.y,
        radius = 3.0;  // for dots

    if (png == NULL) return;
    if (png->polygons == NULL) return;
    int pMax = png->polygons->size();
    if (pMax == 0) return;


    cr->set_source_rgb(1.0, 1.0, 1.0); // white
    cr->paint();  // clear screen
    cr->translate(cx, cy);
    vector<pPGonWork>::iterator sAt = png->polygons->begin() + startDrawAt;
    bool drawBlack = true;
    for (vector<pPGonWork>::iterator it = png->polygons->begin(); it != png->polygons->end(); ++it)
    {
        drawBlack = drawBlack && (it != sAt);
        //cr->set_source_rgb(0.0, 0.0, 0.0);  // black drawing
        pPGonWork wGon = *it;
        pVertexNode v_n = wGon->vNode, v_0 = v_n, v_nxt = NULL;
        cr->set_source(cvClr[13]);
        cr->begin_new_path();
        cr->move_to(v_n->vtxInfo.vertex.x, -v_n->vtxInfo.vertex.y);
        v_n = v_n->next;
        for (unsigned int i = 1; i < wGon->numVertices; ++ i)
        {
            cr->line_to(v_n->vtxInfo.vertex.x, -v_n->vtxInfo.vertex.y);
            v_n = v_n->next;
        }
        cr->close_path();
        cr->fill();
        v_n = v_0;
       for (unsigned int i = 0; i < wGon->numVertices; ++ i)
        {
            v_nxt = v_n->next;
            if (v_nxt == NULL) v_nxt = v_0;
            cr->set_source(cvClr[((v_n->vtxInfo.is_selected)||(v_nxt->vtxInfo.is_selected))? 1: drawBlack? 0: drawColorIndex]);
            cr->move_to(v_n->vtxInfo.vertex.x, -v_n->vtxInfo.vertex.y);
            cr->line_to(v_nxt->vtxInfo.vertex.x, -v_nxt->vtxInfo.vertex.y);
            cr->stroke();
            v_n = v_n->next;
        }
        //cr->set_source(cvClr[v_0->vtxInfo.is_selected? 1: 0]);
        //cr->line_to(v_0->vtxInfo.vertex.x, -v_0->vtxInfo.vertex.y);
        //cr->stroke();
        v_n = v_0;
        for (unsigned int i = 0; i < wGon->numVertices; ++i)
        {
            bool m_s = v_n->vtxInfo.moused;
            cr->set_source(cvClr[m_s? 9: v_n->vtxInfo.is_selected? 1: drawBlack? 0: drawColorIndex]);
            cr->arc(v_n->vtxInfo.vertex.x, -v_n->vtxInfo.vertex.y, radius, 0.0, 2.0 * M_PI);
            cr->fill();
            if (m_s)
            {
                cr->set_source(cvClr[1]);
                cr->arc(v_n->vtxInfo.vertex.x, -v_n->vtxInfo.vertex.y, radius, 0.0, 2.0 * M_PI);
                cr->stroke();
            }
            v_n = v_n->next;
        }
    }
}

void drawgon::draw_dots(void)
{
    if (dots == NULL) return;
    const Cairo::RefPtr<Cairo::Context>& cr = *pCR;
    int width = work_area.get_width(), height = work_area.get_height(),
        min = width < height?width:height;
    double cx = ctr.x, cy = ctr.y,
        radius = 3.0;  // for dots

    cr->set_source_rgb(1.0, 1.0, 1.0); // white
    cr->paint();  // clear screen
    cr->translate(cx, cy);
    cr->set_source(cvClr[drawColorIndex]);
    for (vector<CompGeo::XY>::iterator it = dots->begin(); it != dots->end(); ++it)
    {
        CompGeo::XY xy = *it;
        cr->arc(xy.x, -xy.y, radius, 0.0, 2.0 * M_PI);
        cr->fill();
    }
    //cr->fill();
}

void drawgon::draw_dotsandpolygon(void)
{
    const Cairo::RefPtr<Cairo::Context>& cr = *pCR;
    int width = work_area.get_width(), height = work_area.get_height(),
        min = width < height?width:height;
    double cx = ctr.x, cy = ctr.y,
        radius = 3.0;  // for dots

    cr->set_source_rgb(1.0, 1.0, 1.0); // white
    cr->paint();  // clear screen
    cr->translate(cx, cy);
    cr->set_source(cvClr[drawColorIndex]);
    if (dots != NULL)
    {
        for (vector<CompGeo::XY>::iterator it = dots->begin(); it != dots->end(); ++it)
        {
            CompGeo::XY xy = *it;
            cr->arc(xy.x, -xy.y, radius, 0.0, 2.0 * M_PI);
            cr->fill();
        }
        //cr->fill();
    }

    int pMax = 0;
    bool pgonsPresent = (png != NULL);
    if (pgonsPresent) pgonsPresent = (png->polygons != NULL);
    if (pgonsPresent)
    {
        pMax = png->polygons->size();
        pgonsPresent = (pMax > 0);
    }
    if (pgonsPresent)
    {
        vector<pPGonWork>::iterator sAt = png->polygons->begin() + startDrawAt;
        bool drawBlack = true;
        for (vector<pPGonWork>::iterator it = png->polygons->begin(); it != png->polygons->end(); ++it)
        {
            //cr->set_source_rgb(0.0, 0.0, 0.0);  // black drawing
            drawBlack = drawBlack && (it != sAt);
            pPGonWork wGon = *it;
            pVertexNode v_n = wGon->vNode, v_0 = v_n, v_nxt = NULL;
            cr->set_source(cvClr[13]);
            cr->begin_new_path();
            cr->move_to(v_n->vtxInfo.vertex.x, -v_n->vtxInfo.vertex.y);
            v_n = v_n->next;
            for (unsigned int i = 1; i < wGon->numVertices; ++ i)
            {
                cr->line_to(v_n->vtxInfo.vertex.x, -v_n->vtxInfo.vertex.y);
                v_n = v_n->next;
            }
            cr->close_path();
            cr->fill();
            v_n = v_0;
            for (unsigned int i = 0; i < wGon->numVertices; ++ i)
            {
                v_nxt = v_n->next;
                if (v_nxt == NULL) v_nxt = v_0;
                cr->set_source(cvClr[((v_n->vtxInfo.is_selected)||(v_nxt->vtxInfo.is_selected))? 1: drawBlack? 0: drawColorIndex]);
                cr->move_to(v_n->vtxInfo.vertex.x, -v_n->vtxInfo.vertex.y);
                cr->line_to(v_nxt->vtxInfo.vertex.x, -v_nxt->vtxInfo.vertex.y);
                cr->stroke();
                v_n = v_n->next;
            }
            //cr->set_source(cvClr[v_0->vtxInfo.is_selected? 1: 0]);
            //cr->line_to(v_0->vtxInfo.vertex.x, -v_0->vtxInfo.vertex.y);
            //cr->stroke();
            v_n = v_0;
            for (unsigned int i = 0; i < wGon->numVertices; ++i)
            {
                bool m_s = v_n->vtxInfo.moused;
                cr->set_source(cvClr[m_s? 9: v_n->vtxInfo.is_selected? 1: drawBlack? 0: drawColorIndex]);
                cr->arc(v_n->vtxInfo.vertex.x, -v_n->vtxInfo.vertex.y, radius, 0.0, 2.0 * M_PI);
                cr->fill();
                if (m_s)
                {
                    cr->set_source(cvClr[1]);
                    cr->arc(v_n->vtxInfo.vertex.x, -v_n->vtxInfo.vertex.y, radius, 0.0, 2.0 * M_PI);
                    cr->stroke();
                }
                v_n = v_n->next;
            }
        }
    }
}

void drawgon::draw_dotsandanimation(void)
{ // draws beach line during Voronoi construction
    const Cairo::RefPtr<Cairo::Context>& cr = *pCR;
    int width = work_area.get_width(), height = work_area.get_height(),
        min = width < height?width:height;
    double cx = ctr.x, cy = ctr.y;

    drawColorIndex = 2; //green
    //cr->set_line_width(4.0);
    cr->save();
    draw_dotsandpolygon();
    cr->restore();
    //cr->translate(cx, cy);
    int vFrameIdx = 0;
    vector<CompGeo::XY> * CF = &vFrame[vFrameIdx];
    vector<CompGeo::XY>::iterator fIT = CF->begin();
    CompGeo::XY endPt, endPt2, controlPt1, controlPt2;
    // the frame buffer:
    // 1st, the bezier segments:
    bsegCount = CF->size();
    //cout << "bsegCount:" << to_string(bsegCount);
    if (bsegCount > 0)
    {
        bsegCount = (bsegCount - 1) / 3;
        endPt = CF->at(0);
        cr->move_to(endPt.x + cx, cy - endPt.y);
        //cout << "(" << to_string(endPt.x) << "," << to_string(endPt.y) << ") ";
        for (int i = 0; i < bsegCount; ++i)
        {
            controlPt1 = *(++fIT);
            //cout << "(" << to_string(controlPt1.x) << "," << to_string(controlPt1.y) << ") ";
            controlPt2 = *(++fIT);
            //cout << "(" << to_string(controlPt2.x) << "," << to_string(controlPt2.y) << ") ";
            endPt = *(++fIT);
            //cout << "(" << to_string(endPt.x) << "," << to_string(endPt.y) << ") ";
            cr->curve_to(controlPt1.x + cx, cy - controlPt1.y, controlPt2.x + cx, cy - controlPt2.y, endPt.x + cx, cy - endPt.y);
        }
        //cout << "\n";
        cr->set_source(cvClr[3]);   // blue
        cr->stroke();
    }
    // 2nd, the line segments:
    // the directrix:
    //cout << " the directrix";
    ++vFrameIdx;
    CF = &vFrame[vFrameIdx];
    fIT = CF->begin();
    endPt = *fIT;
    endPt2 = *(++fIT);
    cr->move_to(endPt.x + cx, cy - endPt.y);
    cr->line_to(endPt2.x + cx, cy - endPt2.y);
    cr->set_source(cvClr[9]);   // yellow
    cr->stroke();
    // now, the edges:
    lsegCount = CF->size() / 2;
    //cout << " lsegCount:" << to_string(lsegCount);
    if (lsegCount > 1)
    {
        for (int i = 1; i < lsegCount; ++i)
        {
            endPt = *(++fIT);
            endPt2 = *(++fIT);
            cr->move_to(endPt.x + cx, cy - endPt.y);
            cr->line_to(endPt2.x + cx, cy - endPt2.y);
        }
        cr->set_source(cvClr[0]);   // black
        cr->stroke();
    }
    // 3rd, the site points:
    // get ready to use image with emojis
        //cr->translate(-cx, -cy);  // un-translate
        int w = image->get_width(), h = image->get_height();

        double sf[] = {width / (double)w, height / (double)h};
        int sfIdx = sf[0] < sf[1]? 0: 1;
        double s_f = sf[sfIdx] / 4.0;
        //cout << "Scale Factor: " << to_string(s_f) << "\n";
        cr->scale(s_f, s_f);
        int sx = 78, sy = 364, eWidth = 156, eHeight = 182, dy = 235, dx = 193, 
            nx = 8, ny = 5;

        double CX = cx / s_f, CY = cy / s_f;

    vector<int>::reverse_iterator eIT = V_BMP_IDX.rbegin();
    bool readIdx = eIT != V_BMP_IDX.rend();
    int r = emojiIdx, ry = r / nx, rx = r - nx * ry;
    ++vFrameIdx;
    CF = &vFrame[vFrameIdx];
    fIT = CF->begin();
    sCount = CF->size();
    int biCount = V_BMP_IDX.size(), sDelta = sCount - biCount;
    assert (sCount >= biCount);
    //cout << " sCount:" << to_string(sCount) << "\n";
    for (int i = 0; i < sCount; ++i)
    {
        readIdx = (i >= sDelta);
        //readIdx = (readIdx) && (V_BMP_IDX.rend() != eIT); // ControlInterface needs to clear V_BMP_IDX when animation finishes
        if (readIdx) r = *eIT++;
        else
        {
            r = rand() % (nx * ny);
            V_BMP_IDX.push_back(r);
        }
        ry = r / nx; rx = r - nx * ry;
        CompGeo::XY xyM = *(fIT++); // point on model
        xyM.x /= s_f;
        xyM.y /= s_f;
        CompGeo::XY xyS(xyM.x + CX - eWidth / 2, CY - xyM.y - eHeight / 2); // point on screen
        //cout << to_string(i) + (readIdx? "+":"-") + "(" << to_string(xyM.x) << "," << to_string(xyM.y) << ") (" << to_string(xyS.x) << "," << to_string(xyS.y) << ") ";
        cr->set_source(image, xyS.x - (sx + rx * dx), xyS.y - (sy + ry * dy));
        cr->rectangle(xyS.x, xyS.y, eWidth, eHeight);
        cr->clip();
        cr->paint ();
        cr->reset_clip();
        //if (!readIdx) V_BMP_IDX.push_back(r);
    }
    //cout << "\n";
    // 4th, the attribution:
    string txt = "emojis under design";

    Cairo::FontExtents f_e;
    Cairo::TextExtents t_e;
    cr->scale(1.0 / s_f, 1.0 / s_f);    // reverse scale
    int textcolor = 7;  // dark-pink
    cr->set_source(cvClr[textcolor]);
    cr->set_font_size(width / 80.0);
	cr->select_font_face("Times New Roman", 
        Cairo::FontSlant::FONT_SLANT_ITALIC, 
        Cairo::FontWeight::FONT_WEIGHT_BOLD);
	cr->get_font_extents(f_e);
    cr->get_text_extents(txt, t_e);
	double x = cx - t_e.x_bearing - t_e.width / 2,
	    y = 0.75 * height - f_e.descent + f_e.height / 2;
	cr->move_to(x, y);
	cr->show_text(txt);
   drawColorIndex = 0;  // back to black
    
}

void drawgon::experimental(void)
{
    const Cairo::RefPtr<Cairo::Context>& cr = *pCR;
    int width = work_area.get_width(), height = work_area.get_height(),
        min = width < height?width:height;
    double cx = ctr.x, cy = ctr.y,
        radius = min - 10;

    int bkground = rand() % 13;
    cr->set_source(cvClr[bkground]);
    cr->paint();  // clear screen
    //cr->translate(cx, cy);
    //cr->set_source(cvClr[drawColorIndex]);


int              w, h;
//cairo_surface_t *image;

//cr->begin_new_path();
//cr->new_path (); /* path not consumed by clip()*/


//Glib::RefPtr<Gdk::Pixbuf> image = Gdk::Pixbuf::create_from_file("resources/emoji.png");

//image = cairo_image_surface_create_from_png ("data/romedalen.png");
//w = cairo_image_surface_get_width (image);
//h = cairo_image_surface_get_height (image);

w = image->get_width();
h = image->get_height();

double sf[] = {width / (double)w, height / (double)h};
int sfIdx = sf[0] < sf[1]? 0: 1;
double s_f = sf[sfIdx] / 4.0, TL[] = {cx / s_f - w / 2.0, cy / s_f - h / 2.0};

cr->scale(s_f, s_f);
//cx += TL[0];
//cy += TL[1];
//if (w < min) min = w;
//if (h < min) min = h;
//radius = min / 2 - 10;
//cr->arc (cx, cy, radius, 0, 2*M_PI);
//cr->clip ();
//cr->scale ((double)min/w, (double)min/h);


//cairo_surface_destroy (image);

// draw rectangle:
//int sx = 15, sy = 70, eWidth = 30, eHeight = 35, dy = 43, dx = 36, 
int sx = 78, sy = 364, eWidth = 156, eHeight = 182, dy = 235, dx = 193, 
    nx = 8, ny = 5;

double CX = cx / s_f, CY = cy / s_f;

for (int i = 0; i < 10; ++i)
{
    int r = emojiIdx, ry = r / nx, rx = r - nx * ry;
    double possX = rand() % ((int)(width / s_f) - eWidth), possY = rand() % ((int)(height / s_f) - eHeight);
    CompGeo::XY xyM(possX - CX, CY - possY), // point on model
                xyS(xyM.x + CX - eWidth / 2, CY - xyM.y - eHeight / 2); // point on screen

    cr->set_source(image, xyS.x - (sx + rx * dx), xyS.y - (sy + ry * dy));
    //cairo_set_source_surface (cr, image, 0, 0);
    //cr->paint();
    //cr->set_source(cvClr[0]);
    //cr->move_to(TL[0] + sf[sfIdx] * (sx + rx * dx), TL[1] + sf[sfIdx] * (sy + ry * dy));
    //cr->line_to(TL[0] + sf[sfIdx] * (sx + rx * dx + eWidth), TL[1] + sf[sfIdx] * (sy + ry * dy));
    //cr->line_to(TL[0] + sf[sfIdx] * (sx + rx * dx + eWidth), TL[1] + sf[sfIdx] * (sy + ry * dy + eHeight));
    //cr->line_to(TL[0] + sf[sfIdx] * (sx + rx * dx), TL[1] + sf[sfIdx] * (sy + ry * dy + eHeight));
    //cr->line_to(TL[0] + sf[sfIdx] * (sx + rx * dx), TL[1] + sf[sfIdx] * (sy + ry * dy));
    cr->rectangle(xyS.x, xyS.y, eWidth, eHeight);
    //cr->move_to(TL[0] + (sx + rx * dx), TL[1] + (sy + ry * dy));
    //cr->line_to(TL[0] + (sx + rx * dx + eWidth), TL[1] + (sy + ry * dy));
    //cr->line_to(TL[0] + (sx + rx * dx + eWidth), TL[1] + (sy + ry * dy + eHeight));
    //cr->line_to(TL[0] + (sx + rx * dx), TL[1] + (sy + ry * dy + eHeight));
    //cr->line_to(TL[0] + (sx + rx * dx), TL[1] + (sy + ry * dy));
    //cr->stroke();
    cr->clip();
    cr->paint ();
    cr->reset_clip();
    emojiIdx = rand() % (nx * ny);

}
/*

    int sx = 78, sy = 364, eWidth = 156, eHeight = 182, dy = 235, dx = 193, 
        nx = 8, ny = 5,
        r = emojiIdx, ry = r / nx, rx = r - nx * ry,
        deltax = cx - eWidth / 2 - (sx + rx * dx), 
        deltay = cy - eHeight / 2 - (sy + ry * dy);

    
    //Gdk::Cairo::set_source_pixbuf(cr, image);
    cr->set_source(image, deltax, deltay);
    cr->rectangle(sx + rx * dx, sy + ry * dy, eWidth, eHeight);
    cr->clip();

    cr->paint();

    cr->reset_clip();
 */ 

    cr->reset_clip();
    string txt = "emojis under design";

    Cairo::FontExtents f_e;
    Cairo::TextExtents t_e;


    //Cairo::RefPtr<Cairo::ImageSurface> surface =
    //Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
    
    //cr->create(surface);
    cr->scale(1.0 / s_f, 1.0 / s_f);
    int textcolor = 7;  // dark-pink
    if (bkground == 7) textcolor = 2;   // green
    if (bkground == 11) textcolor = 3;  // blue
    cr->set_source(cvClr[textcolor]); // probably dark-pink
    //cr->paint();
    cr->set_font_size(width / 80.0);
    //Cairo::Matrix cm(width / 50.0, 0, 0, height / 32.0, 0, 0);
    //cr->set_font_matrix(cm);
	cr->select_font_face("Times New Roman", 
        Cairo::FontSlant::FONT_SLANT_ITALIC, 
        Cairo::FontWeight::FONT_WEIGHT_BOLD);
	//cr->get_font_extents(f_e);
    //double ux = 1.0, uy = 1.0, px, py;
	//cr->device_to_user_distance(ux, uy);
	//if (ux > uy)
	//	px = ux;
	//else
	//	px = uy;
	cr->get_font_extents(f_e);
    cr->get_text_extents(txt, t_e);
	double x = cx - t_e.x_bearing - t_e.width / 2,
	    y = 0.75 * height - f_e.descent + f_e.height / 2;
	cr->move_to(x, y);
	//cairo_set_source_rgb (cr, 0, 0, 0);
	cr->show_text(txt);

}


