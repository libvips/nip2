/* gtkplotdt - delaunay triangulization algorithm for gtk+
 * Copyright 2001  Andy Thaller <thaller@ph.tum.de>
 *
 * This library is g_free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <gtk/gtk.h>
#include "gtkplotdt.h"

#if(0)
#define DELAUNAY_DEBUG 
#endif

typedef struct {
  gint a,b;
  GtkPlotDTtriangle *t;
} GtkPlotDTedge;

static void 	gtk_plot_dt_class_init 		(GtkPlotDTClass *klass);
static void 	gtk_plot_dt_init 		(GtkPlotDT *data);
static void 	gtk_plot_dt_destroy		(GtkObject *object);
static gboolean	gtk_plot_dt_real_add_node	(GtkPlotDT *dt,
						 GtkPlotDTnode node);
static GtkPlotDTnode *gtk_plot_dt_real_get_node	(GtkPlotDT *dt,
						 gint idx);
static gboolean	gtk_plot_dt_real_triangulate	(GtkPlotDT *dt);
static void	gtk_plot_dt_real_clear		(GtkPlotDT *dt);
static gboolean	gtk_plot_dt_triangulate_tryquad	(GtkPlotDT *dt);
static void 	gtk_plot_dt_clear_triangles	(GtkPlotDT *data);

static GtkObjectClass *parent_class = NULL;


GtkType
gtk_plot_dt_get_type (void)
{
  static GtkType data_type = 0;

  if (!data_type)
    {
      GtkTypeInfo data_info =
      {
        "GtkPlotDT",
        sizeof (GtkPlotDT),
        sizeof (GtkPlotDTClass),
        (GtkClassInitFunc) gtk_plot_dt_class_init,
        (GtkObjectInitFunc) gtk_plot_dt_init,
        /* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      data_type = gtk_type_unique (GTK_TYPE_OBJECT, &data_info);
    }
  return data_type;
}

static void
gtk_plot_dt_class_init (GtkPlotDTClass *klass)
{
  GtkObjectClass *object_class;
  GtkPlotDTClass *dt_class;

  parent_class = gtk_type_class (gtk_object_get_type ());

  object_class = (GtkObjectClass *) klass;
  dt_class = (GtkPlotDTClass *) klass;

  object_class->destroy = gtk_plot_dt_destroy;

  dt_class->add_node = gtk_plot_dt_real_add_node;
  dt_class->get_node = gtk_plot_dt_real_get_node;
  dt_class->triangulate = gtk_plot_dt_real_triangulate;
  dt_class->clear = gtk_plot_dt_real_clear;
}


/* increase storage size of nodes
 *
 * num: new number of nodes, must be greater that data->node_max
 * returns 0 on failure, 1 on success
 */
static gint
gtk_plot_dt_expand(GtkPlotDT *data, gint num)
{
  GtkPlotDTnode *nodes= 0;
#ifdef DELAUNAY_DEBUG
  fprintf(stderr,"delaunay: expanding to %d\n",num);
#endif
  if (!data) return 0;
  if (!num || num<=data->node_max) return 1;
  nodes= (GtkPlotDTnode*) g_malloc(sizeof(GtkPlotDTnode)*num);
  if (!nodes) return 0;
  if (data->nodes && data->node_cnt)
    memcpy(nodes,data->nodes,sizeof(GtkPlotDTnode)*data->node_cnt);
  if (data->nodes)
    g_free(data->nodes);
  data->nodes= nodes;
  data->node_max= num;
  return 1;
}

/* 
 * num: initial number of nodes
 */

GtkObject*
gtk_plot_dt_new (gint num)
{
  GtkObject *object;

  object = gtk_type_new (gtk_plot_dt_get_type ());

  gtk_plot_dt_expand(GTK_PLOT_DT(object), num);

  return (object);
}

static void
gtk_plot_dt_init (GtkPlotDT *data)
{
  data->quadrilateral= TRUE;
  data->subsampling= FALSE;
  data->nodes= 0;
  data->tmp_nodes= 0;
  data->node_0= 0;
  data->node_cnt= 0;
  data->node_max= 0;

  data->triangles = NULL;
  data->compare_func = NULL;
  data->pbar= 0;
}

static void 
gtk_plot_dt_destroy(GtkObject *object)
{
  GtkPlotDT *data;

  g_return_if_fail(GTK_IS_PLOT_DT(object));

  data = GTK_PLOT_DT(object);

  gtk_plot_dt_clear(data);
}

void 
gtk_plot_dt_clear(GtkPlotDT *data)
{
  GTK_PLOT_DT_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(data)))->clear(data);
}

gboolean 
gtk_plot_dt_add_node(GtkPlotDT *data, GtkPlotDTnode node)
{
  return(GTK_PLOT_DT_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(data)))->add_node(data, node));
}

GtkPlotDTnode * 
gtk_plot_dt_get_node(GtkPlotDT *data, gint idx)
{
  return(GTK_PLOT_DT_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(data)))->get_node(data, idx));
}

gboolean 
gtk_plot_dt_triangulate(GtkPlotDT *data)
{
  return(GTK_PLOT_DT_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(data)))->triangulate(data));
}

static void 
gtk_plot_dt_real_clear(GtkPlotDT *data)
{
  if (!data) return;
  if (data->nodes) g_free(data->nodes); data->nodes= NULL;
  if (data->tmp_nodes) g_free(data->tmp_nodes); data->tmp_nodes= NULL;
  data->node_cnt= data->node_max= data->node_0= 0;

  gtk_plot_dt_clear_triangles(data);

}

static void
gtk_plot_dt_clear_triangles(GtkPlotDT *data)
{ 
  GList *list;

  if(data->triangles){
    for (list= data->triangles; list; list=list->next) 
      if (list->data) g_free(list->data);    
    g_list_free(data->triangles);
    data->triangles= NULL;
  } 
}

static gint
gtk_plot_dt_count_triangles(GtkPlotDT *data)
{ 
  GList *list;
  gint count=0;

  if(data->triangles)
    for (list= data->triangles; list; list=list->next) ++count;
  
  return count;
}



static gboolean 
gtk_plot_dt_real_add_node(GtkPlotDT *data, GtkPlotDTnode node)
{
  gint i;
  GtkPlotDTnode *n;
#ifdef DELAUNAY_DEBUG
  fprintf(stderr,"delaunay: adding node %d\n",data->node_cnt);
#endif
  if (!data) return FALSE;
  for (i=0, n= data->nodes; i<data->node_cnt; i++, n++) {
    /* test for multiple insertions */
    if (n->x==node.x && n->y==node.y && n->z==node.z) {
#ifdef DELAUNAY_DEBUG
      fprintf(stderr,"gtk_plot_dt_add_node(): rejecting multiple node %d\n",
	    data->node_cnt);
#endif
      return FALSE;	
    }
  }
  if (data->node_cnt+1>=data->node_max && 
      !gtk_plot_dt_expand(data,data->node_cnt+10)) {
    fprintf(stderr,"gtk_plot_dt_add_node(): out of memory on node %d\n",
	    data->node_cnt);
    return FALSE;
  }
  n= &data->nodes[data->node_cnt];
  memcpy(n, &node, sizeof(GtkPlotDTnode));
  n->id= data->node_cnt;
  n->a= n->b= n->c= n->d= 0;
  data->node_cnt++;
  return TRUE;
}

static gint 
gtk_plot_dt_circle(GtkPlotDTnode *center, gdouble *r, 
		   GtkPlotDTnode *a, GtkPlotDTnode *b, GtkPlotDTnode *c)
{
  gdouble ax= a->x*a->x;
  gdouble ay= a->y*a->y;
  gdouble bx= b->x*b->x;
  gdouble by= b->y*b->y;
  gdouble cx= c->x*c->x;
  gdouble cy= c->y*c->y;
  gdouble U= bx - ax + by - ay;
  gdouble V= cx - ax + cy - ay;
  gdouble Bx= 2*(b->x-a->x);
  gdouble By= 2*(b->y-a->y);
  gdouble Cx= 2*(c->x-a->x);
  gdouble Cy= 2*(c->y-a->y);
  gdouble x,y;

  if (fabs(Bx)>1e-10) {
    center->y= (V*Bx-U*Cx)/(Bx*Cy-By*Cx);
    center->x= (U-center->y*By)/Bx;
    x= a->x-center->x;
    y= a->y-center->y;
    *r= x*x + y*y;
    return 1;
  } 
  if(fabs(By)>1e-10) {
    center->x= (V*By-U*Cy)/(Cx*By-Bx*Cy);
    center->y= (U-center->x*Bx)/By;
    x= a->x-center->x;
    y= a->y-center->y;
    *r= x*x + y*y;
    return 1;
  } 
  if (fabs(Cx)>1e-20) {
    center->y= (U*Cx-V*Bx)/(By*Cx-Cy*Bx);
    center->x= (V-center->y*Cy)/Cx;
    x= a->x-center->x;
    y= a->y-center->y;
    *r= x*x + y*y;
    return 1;
  } 
  if(fabs(Cy)>1e-20) {
    center->x= (U*Cy-V*By)/(Bx*Cy-Cx*By);
    center->y= (V-center->x*Cx)/Cy;
    x= a->x-center->x;
    y= a->y-center->y;
    *r= x*x + y*y;
    return 1;
  } 
  return 0;
}

static GtkPlotDTnode *
gtk_plot_dt_real_get_node(GtkPlotDT *data, gint idx)
{
  if (!data) return 0;
  if (idx<data->node_0) return 0;
  if (idx<0) return &data->tmp_nodes[-1-idx];
  if (idx<data->node_cnt) return &data->nodes[idx];
  return 0;
}

/* calculates __twice__ the area of a given triangle */
static gdouble
gtk_plot_dt_area_triangle(GtkPlotDTnode *a, GtkPlotDTnode *b, GtkPlotDTnode *c)
{
  if (!a || !b || !c) return 0;
  return fabs((a->x*b->y + a->y*c->x + b->x*c->y -
	       c->x*b->y - c->y*a->x - b->x*a->y));
}

static GtkPlotDTtriangle * 
gtk_plot_dt_add_triangle(GtkPlotDT *data, gint a, gint b, gint c)
{
  GtkPlotDTtriangle *t= 0;
  GtkPlotDTnode *na,*nb,*nc;
  gdouble xmin,xmax,ymin,ymax, orientation;
  if (!data) return 0;
  if (a==b || a==c || b==c) return NULL;

  t= g_new0(GtkPlotDTtriangle, 1);
  if (!t) return NULL;
  t->a= a;
  t->b= b;
  t->c= c;
  t->nn[0] = t->nn[1] = t->nn[2] = NULL;

  na= t->na= gtk_plot_dt_get_node(data,a);
  nb= t->nb= gtk_plot_dt_get_node(data,b);
  nc= t->nc= gtk_plot_dt_get_node(data,c);

  /* make sure the orientation of the new triangle is positive: */
  orientation= (nb->x-na->x)*(nc->y-na->y) - (nb->y-na->y)*(nc->x-na->x);
  if (orientation<0) {
    t->nc= nb; t->nb= nc;
    t->c= b; t->b= c;
    /* if this ever turns up, enclose it in ifdef DELAUNAY_DEBUG ! */
    fprintf(stderr,"corrected orientation of new triangle\n");
  }

  /* create bounding-box */
  xmin= xmax= na->x;
  ymin= ymax= na->y;
  if (xmax< nb->x) xmax= nb->x; else if (xmin>nb->x) xmin= nb->x;
  if (ymax< nb->y) ymax= nb->y; else if (ymin>nb->y) ymin= nb->y;
  if (xmax< nc->x) xmax= nc->x; else if (xmin>nc->x) xmin= nc->x;
  if (ymax< nc->y) ymax= nc->y; else if (ymin>nc->y) ymin= nc->y;

  t->min.x= xmin;
  t->min.y= ymin;
  t->max.x= xmax;
  t->max.y= ymax;
 
  t->area= 0.0; 
  t->radius= -1.0;
  /* prepending gives the same result in the end but >twice as fast */
  data->triangles = g_list_prepend(data->triangles, t);
#ifdef DELAUNAY_DEBUG
  fprintf(stderr,"added triangle : %d/%d/%d c=(%g,%g) r=%g\n",a,b,c,
	  t->ccenter.x,t->ccenter.y, t->radius);
#endif
  return t;
}

static gint 
gtk_plot_dt_update_tmpnodes(GtkPlotDT *data)
{
  gint i;
  gdouble xmin,xmax,ymin,ymax,delta=0.5;
  if (!data || data->node_cnt<3) return 0;

  /* determine bounding box */

  xmin= xmax= data->nodes[0].x;
  ymin= ymax= data->nodes[0].y;
  for (i=1; i<data->node_cnt; i++) {
    if (xmax<data->nodes[i].x) xmax=data->nodes[i].x;
    if (xmin>data->nodes[i].x) xmin=data->nodes[i].x;
    if (ymax<data->nodes[i].y) ymax=data->nodes[i].y;
    if (ymin>data->nodes[i].y) ymin=data->nodes[i].y;
  }
#ifdef DELAUNAY_DEBUG
  fprintf(stderr,"data confinement: [%g:%g] [%g:%g]\n",xmin,xmax,ymin,ymax);
#endif

  /* some practical magic: have two triangles which cover _all_ the nodes */

  if (data->tmp_nodes) g_free(data->tmp_nodes);
  data->tmp_nodes= g_new0(GtkPlotDTnode, 8);
  data->node_0= -8;
  if (!data->tmp_nodes) return 0;

  data->tmp_nodes[0].x= xmin-delta*(xmax-xmin);
  data->tmp_nodes[0].y= ymin-delta*(ymax-ymin);
  data->tmp_nodes[1].x= xmax+delta*(xmax-xmin);
  data->tmp_nodes[1].y= ymin-delta*(ymax-ymin);
  data->tmp_nodes[2].x= xmax+delta*(xmax-xmin);
  data->tmp_nodes[2].y= ymax+delta*(ymax-ymin);
  data->tmp_nodes[3].x= xmin-delta*(xmax-xmin);
  data->tmp_nodes[3].y= ymax+delta*(ymax-ymin);

  gtk_plot_dt_add_triangle(data,-1,-2,-3);
  gtk_plot_dt_add_triangle(data,-1,-3,-4);

  return 1;
}

static gint 
gtk_plot_dt_drop_tmpstuff(GtkPlotDT *data)
{
  GtkPlotDTtriangle *t;
  GList *list = NULL, *next = NULL;
  gint tri_cnt = 0;
  gint i;

  if (!data) return 0;

  list = data->triangles;  
  while(list) {
    t= (GtkPlotDTtriangle *)list->data;

    for(i = 0; i < 3; i++)
      if(t->nn[i] && (t->nn[i]->a < 0 || t->nn[i]->b < 0 || t->nn[i]->c < 0)) t->nn[i] = NULL;
    list = list->next;
  }

  list = data->triangles;  
  while(list) {
    next= list->next;
    t= (GtkPlotDTtriangle *)list->data;

    if (t->a<0 || t->b<0 || t->c<0) {
#ifdef DELAUNAY_DEBUG
      fprintf(stderr,"dropping triangle %d/%d/%d\n",t->a,t->b,t->c);
#endif
      data->triangles = g_list_remove_link(data->triangles, list);
      g_free(list->data);
      g_list_free_1(list);
    } else {
      tri_cnt++;
    }
    list = next;
  }
  g_free(data->tmp_nodes);
  data->tmp_nodes= NULL; data->node_0= 0;

  return tri_cnt;
}


/* checks if the node is inside the triangle's empty circle or not */
static gboolean 
gtk_plot_dt_inside_triangle_circle(GtkPlotDTtriangle *t, GtkPlotDTnode *n)
{
  gdouble r,x,y;
  if (!t || !n) return FALSE;
  if (t->radius < 0.0) {
    gtk_plot_dt_circle(&t->ccenter,&t->radius,t->na,t->nb,t->nc);
  }
  x= n->x-t->ccenter.x;
  y= n->y-t->ccenter.y;
  r= x*x + y*y;
#ifdef DELAUNAY_DEBUG2
  fprintf(stderr,"gtk_plot_dt_inside_triangle_circle : %g < %g = %d\n",
  	  r,t->radius,(r < t->radius));
#endif
  if (r < t->radius) return TRUE;
  
  return FALSE;
}

/* checks if the node is inside the triangle or not */
static gdouble
gtk_plot_dt_inside_triangle(GtkPlotDT *data, GtkPlotDTtriangle *t, 
			    GtkPlotDTnode *n)
{
  gdouble A,B;
  GtkPlotDTnode *a,*b,*c;
  if (!t || !n) return 1e99;
  /* test boundary box first */
  
  if ((n->x < t->min.x) || (n->x > t->max.x) ||
      (n->y < t->min.y) || (n->y > t->max.y)) return 1e99;
  
  a= t->na;
  b= t->nb;
  c= t->nc;
  /* calculate triangle's area if it's not yet known: */
  A= t->area;
  if (A==0.0) A= t->area= gtk_plot_dt_area_triangle(a,b,c);
  B= gtk_plot_dt_area_triangle(n,a,b)+
     gtk_plot_dt_area_triangle(n,b,c)+
     gtk_plot_dt_area_triangle(n,c,a);
  /*
  fprintf(stderr,"gtk_plot_dt_inside_triangle %d/%d/%d: %g > %g = %d! "
	  "(%ld,%ld,%ld)\n",t->a,t->b,t->c,B,A,(B>A),a,b,c);
  */
  return B/A; /* A cannot be 0 - see gtk_plot_dt_add_triangle()! */
}

static gboolean
edges_equal(GtkPlotDTedge *e1, GtkPlotDTedge *e2)
{
  if((e1->a == e2->a && e1->b == e2->b) || (e1->b == e2->a && e1->a == e2->b)) return TRUE;
  return FALSE;
}

static void
gtk_plot_dt_triangulate_insert_node(GtkPlotDT *data, GtkPlotDTnode *node)
{
  gint j,k,l, delinquentes;
  GtkPlotDTtriangle *t = NULL;
  gdouble err, min;
  GList *list = NULL, *doomed = NULL, *last = NULL, *new = NULL, *aux = NULL;
  gint num = 0;
  GtkPlotDTedge *edges = NULL;

#ifdef DELAUNAY_DEBUG
  printf("inserting node %d %f %f\n",node->id,node->x,node->y);
#endif
  /* first, find the triangle that contains this node: 
   */
  num = 0;
  delinquentes = 0;
  doomed = NULL; 
  min= 1e99; /* minimal inside-triangle criteria found (==1: inside) */
  list = data->triangles;
  while(list) {
    t = (GtkPlotDTtriangle *)list->data;
    if ((err=gtk_plot_dt_inside_triangle(data,t,node))
	<min) {
      min= err; 
      doomed = list;
      if (min<1.000001) break; /* need not be better than that! */
    }
    list = list->next;
  }
  
  if (doomed) { /* triangle found with node inside! */
    GList *last_doomed;
    /* "doomed" is a new list where all removed triangles go to
     * for finding the outline of the new triangles to be inserted.
     * The first triangle to go there is the one found above.
     * Afterwards the "doomed" list is doomed to die :-)
     */
    data->triangles= g_list_remove_link(data->triangles, doomed);
    last_doomed = g_list_last(doomed);
    delinquentes++;
#ifdef DELAUNAY_DEBUG
    fprintf(stderr,"marking triangle %d/%d/%d doomed (min=%g)\n",
	    t->a,t->b,t->c,min);
#endif
    /* now, find all adjacent triangles with this node in their circles: */
    list = data->triangles;
    while(list) {
      GtkPlotDTtriangle *doomed_t;
      t = (GtkPlotDTtriangle *)list->data;
      doomed_t = (GtkPlotDTtriangle *)doomed->data;
      if ((t->a==doomed_t->a || t->b==doomed_t->a || t->c==doomed_t->a || 
	   t->a==doomed_t->b || t->b==doomed_t->b || t->c==doomed_t->b || 
	   t->a==doomed_t->c || t->b==doomed_t->c || t->c==doomed_t->c) &&
	  gtk_plot_dt_inside_triangle_circle(t,node)) {
	
	/* move triangle to "doomed" list */
	last= list->next;
	data->triangles= g_list_remove_link(data->triangles, list);
        /* faster than g_list_concat */
        list->prev = last_doomed;
        list->next = NULL;
        last_doomed->next = list;
        last_doomed = list;
	delinquentes++;
#ifdef DELAUNAY_DEBUG
	fprintf(stderr,"marking     also %d/%d/%d doomed\n",t->a,t->b,t->c);
#endif
	list = last;
      } else {
	list = list->next;
      }
    }
    edges= (GtkPlotDTedge*)g_malloc(sizeof(GtkPlotDTedge)*delinquentes*3);
    num= 0;
    
    list = doomed;
    while(list){
      t = (GtkPlotDTtriangle *)list->data;
      edges[num].a= t->a; edges[num].b= t->b; edges[num].t = t; num++;
      edges[num].a= t->b; edges[num].b= t->c; edges[num].t = t; num++;
      edges[num].a= t->c; edges[num].b= t->a; edges[num].t = t; num++;
      list = list->next;
    }

#ifdef DELAUNAY_DEBUG
    fprintf(stderr,"detected edges: ");
    for (j=0; j<num; j++)
      fprintf(stderr,"(%d %d) ",edges[j].a,edges[j].b);
    fprintf(stderr,"\n");
    fprintf(stderr,"collapsing edges: ");
    for (j=0; j<num; j++) {
      l=0; 
      for (k=0; k<num; k++) 
	if (edges_equal(&edges[j], &edges[k])) l++;
      if (l==1)
	fprintf(stderr,"(%d %d) ",edges[j].a,edges[j].b);
    }
    fprintf(stderr,"\n");
#endif
    /* for all edges: */
    for (j=0; j<num; j++) {
      /* count occurences of this edge: */ 
      for (k=0, l=0; k<num; k++) 
	if (edges_equal(&edges[j], &edges[k])) l++;
      /* only one occurence of an edge indicates outline edge! */
      if (l==1){ 
        GtkPlotDTtriangle *t;
	t = gtk_plot_dt_add_triangle(data,node->id,edges[j].a,edges[j].b);
        if(t) new = g_list_append(new, t);
      }
    }
    if(edges) g_free(edges);


    /* Find neighbours */
    list = new;
    while(list){
      GtkPlotDTtriangle *t = (GtkPlotDTtriangle *)list->data;
      GtkPlotDTedge t_edges[3];
      gint l, m, n;
      gboolean found = FALSE;
      t_edges[0].a = t->a;
      t_edges[0].b = t->b;
      t_edges[1].a = t->b;
      t_edges[1].b = t->c;
      t_edges[2].a = t->c;
      t_edges[2].b = t->a;
      /* I look for the neighbours into the doomed triangles' neighbours */
      aux = doomed;
      while(aux){
        GtkPlotDTedge nn[3];
        GtkPlotDTtriangle *doomed_t = (GtkPlotDTtriangle *)aux->data;
        /* for each doomed triangles I have 3 neighbours */
        for(l = 0; l < 3; l++){
          GtkPlotDTtriangle *doomed_nn = doomed_t->nn[l];
          GList *aux2; /* verify that doomed_nn is not doomed */
          for(aux2 = doomed; aux2; aux2 = aux2->next) 
            if(doomed_nn == aux2->data) doomed_nn = NULL;
          if(doomed_nn){
            nn[0].a = doomed_nn->a;
            nn[0].b = doomed_nn->b;
            nn[1].a = doomed_nn->b;
            nn[1].b = doomed_nn->c;
            nn[2].a = doomed_nn->c;
            nn[2].b = doomed_nn->a;
            /* for each of the 3 neighbours I have 3 edges */
            for(m = 0; m < 3; m++){
              for(n = 0; n < 3; n++){
                if(edges_equal(&nn[m], &t_edges[n])){
                  doomed_nn->nn[m] = t;
                  t->nn[n] = doomed_nn;
                  found = TRUE;
                  break; 
                }
              }
              if(found) break;
            }
          }
          if(found) break;
        }
        if(found) break;
        aux = aux->next;
      }
      /* I look for the triangle's neighbours into the new triangles */
      aux = new;
      while(aux){
        GtkPlotDTedge nn[3];
        GtkPlotDTtriangle *new_t = (GtkPlotDTtriangle *)aux->data;
        if(new_t != t){
          nn[0].a = new_t->a;
          nn[0].b = new_t->b;
          nn[1].a = new_t->b;
          nn[1].b = new_t->c;
          nn[2].a = new_t->c;
          nn[2].b = new_t->a;
          /* for each of the triangles I have 3 edges */
          found = FALSE;
          for(m = 0; m < 3; m++){
            for(n = 0; n < 3; n++){
              if(edges_equal(&nn[m], &t_edges[n])){
                new_t->nn[m] = t;
                t->nn[n] = new_t;
                found = TRUE;
                break; 
              }
              if(found) break;
            }
            if(found) break;
          }
        }
        aux = aux->next;
      }

      list = list->next;
    }
  }

  /* now actually delete the "doomed" triangles,
   * we don't need them any longer:
   */
  if(doomed){
    for (list = doomed; list; list = list->next){
      GtkPlotDTtriangle *t = (GtkPlotDTtriangle *)list->data;
      gint i;
      if(t->nn[0]){
        for(i = 0; i < 3; i++)
          if(t->nn[0]->nn[i] == t) t->nn[0]->nn[i] = NULL;
      }
      if(t->nn[1]){
        for(i = 1; i < 3; i++)
          if(t->nn[1]->nn[i] == t) t->nn[1]->nn[i] = NULL;
      }
      if(t->nn[2]){
        for(i = 2; i < 3; i++)
          if(t->nn[2]->nn[i] == t) t->nn[2]->nn[i] = NULL;
      }
      if (list->data) g_free(list->data);    
    }
    g_list_free(doomed);
  } 
  if(new) g_list_free(new);
}

/* Not used
static GList *
gtk_plot_dt_triangle_list_copy(GtkPlotDT *data)
{
  GList *list= g_list_copy(data->triangles);
  GList *next;
  GtkPlotDTtriangle *t;
  for (next=list; next; next=next->next) {
    t= g_new(GtkPlotDTtriangle,1);
    memcpy(t,next->data,sizeof(GtkPlotDTtriangle));
  }
  return list;
}
*/

static void 
gtk_plot_dt_create_center_node_4(GtkPlotDTnode *x, 
				 GtkPlotDTnode *a, 
				 GtkPlotDTnode *b, 
				 GtkPlotDTnode *c, 
				 GtkPlotDTnode *d)
{
  if (!x || !a || !b || !c || !d) return;
  x->x= (a->x+b->x+c->x+d->x)/4.;
  x->y= (a->y+b->y+c->y+d->y)/4.;
  x->z= (a->z+b->z+c->z+d->z)/4.;
  x->px= (a->px+b->px+c->px+d->px)/4.;
  x->py= (a->py+b->py+c->py+d->py)/4.;
  x->pz= (a->pz+b->pz+c->pz+d->pz)/4.;
  x->a= x->b= x->c= x->d= 0;
}

/* gtk_plot_dt_triangle_subsample()
 *
 * first, we have to check if triangles t1 and t2 have a common edge, 
 * i.e. two common node indices: the node indices of each triangle t1 
 * and t2 are held in u[] and v[], respectively. First, we check all
 * u[i] against v[i] - if we find two identical values, we're done. If
 * not, we permutate v[] and try again until we're through will all 
 * permutations. If we still didn't find anything, the triangles don't 
 * have a common edge and we return 0.
 * 
 * If they have a common edge, we already saved the four outline points 
 * of the two triangles (giving a polygon with 4 nodes) in a,b,c,d.
 * 
 * All we have to do now is create a new node in the center.
 */
static GtkPlotDTnode *
gtk_plot_dt_triangle_subsample(GtkPlotDT *data,
			       GtkPlotDTtriangle *t1,
			       GtkPlotDTtriangle *t2)
{
  gint i,tmp;
  GtkPlotDTnode *n= NULL;
  gint a= 0,b= 0,c= 0,d= 0, found= 0;
  gint u[3], v[3];

  u[0] = t1->a;
  u[1] = t1->b;
  u[2] = t1->c;
  v[0] = t2->a;
  v[1] = t2->b;
  v[2] = t2->c;

  for (i=0; i<4; i++) {
    if (u[0]==v[0] && u[1]==v[2]) {
      a= u[0]; b=v[1]; c=u[1]; d= u[2]; found++; break;
    }
    if (u[1]==v[1] && u[2]==v[0]) {
      a= u[0]; b=u[1]; c=v[2]; d= u[2]; found++; break;
    }
    if (u[0]==v[0] && u[2]==v[1]) {
      a= u[0]; b=u[1]; c=u[2]; d= v[2]; found++; break;
    }    
    /* permutate nodelist */
    tmp= v[0]; v[0]= v[1]; v[1]= v[2]; v[2]= tmp; 
  }
  if (found) {
    n= g_new(GtkPlotDTnode,1);
    gtk_plot_dt_create_center_node_4(n,
				     gtk_plot_dt_get_node(data,a),
				     gtk_plot_dt_get_node(data,b),
				     gtk_plot_dt_get_node(data,c),
				     gtk_plot_dt_get_node(data,d));
  }
  return n;
}


/* prerequisite: data->tmp_nodes[] must be empty! If this is a problem,
 * it must be made sure that the old content is kept and additional entries
 * have not-yet-used indices!
 */
static gint
gtk_plot_dt_triangulate_subsample(GtkPlotDT *data)
{
  gint node_idx, cnt;
  GList *tmplist, *t, *tmpnodes=NULL;
  GtkPlotDTnode *n= NULL;

  /* for each triangle: walk through the rest of the list, if the triangle
   * and one of the rest are neighbours, store interpolated node 
   */
  cnt= 0;
  for (tmplist= data->triangles; tmplist; tmplist=tmplist->next) {
    for (t= tmplist->next; t; t=t->next) {
      /* if the two triangles are neighbours, n will be the subsampled node */
      n=gtk_plot_dt_triangle_subsample(data,tmplist->data,t->data);
      if (n) {
	tmpnodes= g_list_prepend(tmpnodes,n); 
	cnt++;
      }
    }
  }
  data->tmp_nodes= g_new(GtkPlotDTnode,cnt);
  for (t= tmpnodes, node_idx=0; 
       t && node_idx<cnt;
       t=t->next, node_idx++) {
    n= tmpnodes->data; n->id= -1-node_idx;
    memcpy(&(data->tmp_nodes[node_idx]),n,sizeof(GtkPlotDTnode));
  }
  for (t= tmpnodes; t; t=t->next) { g_free(t->data); }
  g_list_free(tmpnodes);
  data->node_0= -1-cnt;
  for (node_idx=data->node_0;node_idx<0;node_idx++) 
    gtk_plot_dt_triangulate_insert_node(data, 
					gtk_plot_dt_get_node(data,node_idx));
  return gtk_plot_dt_count_triangles(data);
}

static gboolean
gtk_plot_dt_real_triangulate(GtkPlotDT *data)
{
  gint node_idx, tcnt;
  gdouble max; 

  if (!data) return FALSE;
  if (!data || ! data->nodes || data->node_cnt<3) return FALSE;

  gtk_plot_dt_clear_triangles(data);

  if (data->quadrilateral && gtk_plot_dt_triangulate_tryquad(data)) 
    return gtk_plot_dt_count_triangles(data);

  /* create auxiliary triangles all to-be-inserted nodes will reside in */
  if (!gtk_plot_dt_update_tmpnodes(data)) return FALSE;

  /* now insert all nodes in the nodelist */
  max= data->node_cnt-1; max*=max;
  for (node_idx=0; node_idx<data->node_cnt; node_idx++) {
#ifdef DELAUNAY_DEBUG
    fprintf(stderr,"inserting node %d/%d\n",node_idx,data->node_cnt);
#endif
    gtk_plot_dt_triangulate_insert_node(data, &data->nodes[node_idx]);
    if (data->pbar) {
      (*data->pbar) (1.0*node_idx*node_idx/max);
    }
  }

  /* remove auxiliary triangles: */
  tcnt= gtk_plot_dt_drop_tmpstuff(data);

/*
  {
    GList *list = data->triangles;
    while(list){
      GtkPlotDTtriangle *t = (GtkPlotDTtriangle *)list->data;
      printf("TRIANGLE %d %d %d\n",t->a,t->b,t->c);
      if(t->nn[0])
      printf("NN0 : %d %d %d\n",t->nn[0]->a,t->nn[0]->b,t->nn[0]->c);
      if(t->nn[1])
      printf("NN1 : %d %d %d\n",t->nn[1]->a,t->nn[1]->b,t->nn[1]->c);
      if(t->nn[2])
      printf("NN2 : %d %d %d\n",t->nn[2]->a,t->nn[2]->b,t->nn[2]->c);
      
      list = list->next;
    }
  }
*/

  /* if we don't have any triangles left or we don't want subsampling,
   * we're done here 
   */
  if (!tcnt || !data->subsampling) return tcnt;
  
  return gtk_plot_dt_triangulate_subsample(data);
}

static gint
gtk_plot_dt_compare_nodes_x_wise(gconstpointer _a, gconstpointer _b)
{
  GtkPlotDTnode *a= (GtkPlotDTnode *)_a;
  GtkPlotDTnode *b= (GtkPlotDTnode *)_b;
  double dx;
  
  /* find relative difference of x values */
  if (b->x != 0.0) dx= fabs((a->x/b->x) - 1.0); 
  else if (a->x != 0.0) dx= fabs((b->x/a->x) - 1.0);
  else dx= 0.0;
  
  /* first, lets see if two values differ by less than a certain amount */
  if (dx < 1e-10)
    return 0;
  else if (a->x < b->x)
    /* ax < bx */
    return -1;
  else 
    /* ax > bx */
    return  1;
  /* just in case: */
  fprintf(stderr,"gtk_plot_dt_compare_nodes_x_wise(): internal error!\n");
  return 0;
}

static gint
gtk_plot_dt_compare_nodes_y_wise(gconstpointer _a, gconstpointer _b)
{
  GtkPlotDTnode *a= (GtkPlotDTnode *)_a;
  GtkPlotDTnode *b= (GtkPlotDTnode *)_b;
  double dy;
  
  /* find relative difference of y values */
  if (b->y != 0.0) dy= fabs((a->y/b->y) - 1.0);
  else if (a->y != 0.0) dy= fabs((b->y/a->y) - 1.0);
  else dy= 0.0;
    
  if (dy < 1e-10)
    /* ay == by */
    return 0;
  else if (a->y < b->y) 
    /* ay < by */
    return -1;
  else 
    /* ay > by */
    return  1;

  /* just in case: */
  fprintf(stderr,"gtk_plot_dt_compare_nodes_y_wise(): internal error!\n");
  return 0;
}

static gint
gtk_plot_dt_compare_nodes_xy_wise(gconstpointer a, gconstpointer b)
{
  switch(gtk_plot_dt_compare_nodes_x_wise(a,b)) {
  case -1:
    return -1; break;
  case  1: 
    return  1; break;
  case  0: 
    switch(gtk_plot_dt_compare_nodes_y_wise(a,b)) {
    case -1: 
      return -1; break;
    case  1: 
      return  1; break;
    case  0:
      return  0; break;
    }
    break;
  }
  
  /* just in case: */
  fprintf(stderr,"gtk_plot_dt_compare_nodes_xy_wise(): internal error!\n");
  return 0;
}

static GtkPlotDTtriangle *
find_neighbor(GtkPlotDT *dt, GtkPlotDTtriangle *t, gint n1, gint n2)
{
  GList *list;
  GtkPlotDTtriangle *triangle;

  list = dt->triangles;
  while(list) {
    triangle = (GtkPlotDTtriangle *)list->data;
    if(triangle != t){
      if(triangle->na->id == n1 && triangle->nb->id == n2) return triangle;
      if(triangle->nb->id == n1 && triangle->na->id == n2) return triangle;
      if(triangle->na->id == n1 && triangle->nc->id == n2) return triangle;
      if(triangle->nc->id == n1 && triangle->na->id == n2) return triangle;
      if(triangle->nb->id == n1 && triangle->nc->id == n2) return triangle;
      if(triangle->nc->id == n1 && triangle->nb->id == n2) return triangle;
    }
    list = list->next;
  }
  return NULL;
}

static gboolean
gtk_plot_dt_triangulate_tryquad(GtkPlotDT *data)
{
  GList *nodes= NULL, *list= NULL;
  GList *xrow_prev, *xrow_next= NULL, *xrow_sec= NULL;
  GtkPlotDTnode *node, *x0y0, *x0y1, *x1y0, *x1y1, *center;
  gint i, nc, nx= 0, ny= 0, x, y, num_mid, m;
  double x0;
 
  if (!data) return 0;
  if (!data || ! data->nodes || data->node_cnt<3) return 0;

  nc= data->node_cnt;
  node= data->nodes;
  for (i=0; i<nc; i++,node++) {
    nodes= g_list_insert_sorted(nodes,node,gtk_plot_dt_compare_nodes_xy_wise);
  }

  x0= ((GtkPlotDTnode *)nodes->data)->x;

  /* sort order: x0y0 x0y1 x0y2 .. x1y0 x1y1 x1y2 .. xnyn */
  for (list=nodes, i=0; list; list=list->next, i++) {
    if (!gtk_plot_dt_compare_nodes_x_wise(nodes->data, list->data)) 
      ny++;
    else if (!xrow_sec) xrow_sec= list;
  }

  /* if we have only data in one row, we cannot go on: */
  if (ny<2 || !xrow_sec) {
#ifdef DELAUNAY_DEBUG
    fprintf(stderr,"the grid is not quadrilateral (single row)!\n");
#endif
    g_list_free(nodes);
    data->quadrilateral = FALSE;
    return FALSE;
  }

  nx= nc/ny;
  if (nc!= nx*ny) {
#ifdef DELAUNAY_DEBUG
    fprintf(stderr,"the grid is not quadrilateral (%dx%d != %d)!\n",nx,ny,nc);
#endif
    g_list_free(nodes);
    data->quadrilateral = FALSE;
    return FALSE;
  }

  for (xrow_prev= nodes, xrow_next= xrow_sec;
       xrow_next; 
       xrow_prev= xrow_prev->next, xrow_next= xrow_next->next) {
    if (gtk_plot_dt_compare_nodes_y_wise(xrow_prev->data, xrow_next->data)) {
#ifdef DELAUNAY_DEBUG
      fprintf(stderr,"the grid is not quadrilateral!\n");
#endif
      g_list_free(nodes);
      data->quadrilateral = FALSE;
      return FALSE;
    }
  }
  num_mid= (nx-1)*(ny-1);
#ifdef DELAUNAY_DEBUG
  fprintf(stderr,"the grid is quadrilateral %dx%d, inserting %d nodes!\n",
	  nx,ny,num_mid);
#endif

  /* now create center nodes and triangle list: */  
  data->tmp_nodes= g_new(GtkPlotDTnode, num_mid);
  data->node_0= -num_mid;

  for (xrow_prev= nodes, xrow_next= xrow_sec, x=0, y=0, m=0;
       xrow_next && xrow_next->next;
       xrow_prev= xrow_prev->next, xrow_next= xrow_next->next) {
    if (x<nx-1) {
      if (y<ny-1) {
        GtkPlotDTtriangle *t1, *t2, *t3, *t4;
	x0y0= xrow_prev->data;
	x0y1= xrow_prev->next->data;
	x1y0= xrow_next->data;
	x1y1= xrow_next->next->data;
	center= data->tmp_nodes+m;
	gtk_plot_dt_create_center_node_4(center,x0y0,x0y1,x1y0,x1y1);
	center->id= -1-m;
	center->a= x0y0->id;
	center->b= x0y1->id;
	center->c= x1y0->id;
	center->d= x1y1->id;
	t1 = gtk_plot_dt_add_triangle(data,-1-m,x0y0->id,x1y0->id);
	t2 = gtk_plot_dt_add_triangle(data,-1-m,x1y0->id,x1y1->id);
	t3 = gtk_plot_dt_add_triangle(data,-1-m,x1y1->id,x0y1->id);
	t4 = gtk_plot_dt_add_triangle(data,-1-m,x0y1->id,x0y0->id);
        t1->nn[0] = t4;
        t1->nn[1] = t2;
        t2->nn[0] = t1;
        t2->nn[1] = t3;
        t3->nn[0] = t2;
        t3->nn[1] = t4;
        t4->nn[0] = t3;
        t4->nn[1] = t1;
        find_neighbor(data, t1, x0y0->id, x1y0->id);
        find_neighbor(data, t2, x1y0->id, x1y1->id);
        find_neighbor(data, t3, x1y1->id, x0y1->id);
        find_neighbor(data, t4, x0y1->id, x0y0->id);
	m++;
	y++;
      } else {
	x++; y= 0;
      }
    } else {
      fprintf(stderr,"gtk_plot_dt_triangulate_tryquad(): internal error!\n");
    }
  }

  g_list_free(nodes);
  return TRUE;
}

void
gtk_plot_dt_set_quadrilateral(GtkPlotDT *dt, gboolean set)
{
  gboolean old = dt->quadrilateral;
  dt->quadrilateral = set;
  if(dt->triangles && old!=set) gtk_plot_dt_triangulate(dt);
} 

void
gtk_plot_dt_set_subsampling(GtkPlotDT *dt, gboolean set)
{
  gboolean old = dt->subsampling;
  dt->subsampling = set;
  if(dt->triangles && old!=set) gtk_plot_dt_triangulate(dt);
} 
