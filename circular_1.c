#include <stdio.h>
#include <SDL.h>

#define WINDOW_WIDTH 1000
#define RADIUS 250
#define AREA_INACCURACY 1.0f

#define MAXIMUM_OBJECT_IN_SPACE 1

#define SPACE_LENGTH 25

#define DISCRETE_TIME_PACKET_VALUE 0.1f

struct VECTOR_2D {
    float X;
    float Y;
};

struct OBJECT {
    struct VECTOR_2D LOCATION;
    struct VECTOR_2D VELOCITY;
    struct VECTOR_2D NET_FORCE;
    float MASS;
};

void f(void *x){
    int i;
    struct OBJECT *Space = (struct OBJECT *) x;
    while((Space - (struct OBJECT *) x) < MAXIMUM_OBJECT_IN_SPACE){
        Space->LOCATION.X += DISCRETE_TIME_PACKET_VALUE * Space->VELOCITY.X;
        Space->LOCATION.Y += DISCRETE_TIME_PACKET_VALUE * Space->VELOCITY.Y;
        Space->VELOCITY.X += DISCRETE_TIME_PACKET_VALUE * Space->NET_FORCE.X / Space->MASS;
        Space->VELOCITY.Y += DISCRETE_TIME_PACKET_VALUE * Space->NET_FORCE.Y / Space->MASS;
        ++Space;
    }
}

struct COLOR_MIX {
	unsigned char RED;
	unsigned char GREEN;
	unsigned char BLUE;
};

struct POINT_3D_FLOAT {
	float X;
	float Y;
	float Z;
};

void normal_vector(struct POINT_3D_FLOAT *p, struct POINT_3D_FLOAT *q, struct POINT_3D_FLOAT *r, float *a, float *b, float *c){
	*a = -(r->Y - p->Y) * (q->Z - p->Z) + (q->Y - p->Y) * (r->Z - p->Z);
	*b = -(q->X - p->X) * (r->Z - p->Z) + (r->X - p->X) * (q->Z - p->Z);
	*c = -(r->X - p->X) * (q->Y - p->Y) + (q->X - p->X) * (r->Y - p->Y);
}

float area_triangle(struct POINT_3D_FLOAT *p, struct POINT_3D_FLOAT *q, struct POINT_3D_FLOAT *r){
	float a, b, c;
	normal_vector(p, q, r, &a, &b, &c);
	float result = 0.5 * sqrt(a*a + b*b + c*c);
	return result;
}

int inside_triangle(struct POINT_3D_FLOAT *triangle, struct POINT_3D_FLOAT *s){
	float a = area_triangle(triangle, triangle + 1, s);
	float b = area_triangle(triangle + 1, triangle + 2, s);
	float c = area_triangle(triangle + 2, triangle, s);
	float d = a + b + c - area_triangle(triangle, triangle + 1, triangle + 2);
	if (d < 0.0f) d= -d;
	if (d < AREA_INACCURACY){
		return 1;
	}
	return 0;
}


void draw_buffer(SDL_Renderer *renderer, struct COLOR_MIX *frame_buffer){
	SDL_Texture *Tile = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, RADIUS*2, RADIUS*2);
	unsigned char *bytes;
    int pitch;
    SDL_LockTexture(Tile, NULL, (void **)&bytes, &pitch);
    int i, j;
    for (i=0; i<RADIUS*2; ++i){
    	for (j=0; j<RADIUS*2; ++j){
    		bytes[(i*RADIUS*8+j*4)] = 255;
    		bytes[(i*RADIUS*8+j*4) + 1] = frame_buffer[j*RADIUS*2+i].BLUE;
    		bytes[(i*RADIUS*8+j*4) + 2] = frame_buffer[j*RADIUS*2+i].GREEN;
    		bytes[(i*RADIUS*8+j*4) + 3] = frame_buffer[j*RADIUS*2+i].RED;
    	}
    }
    SDL_UnlockTexture(Tile);
    SDL_Rect destination = {0, 0, RADIUS*2, RADIUS*2};
    SDL_RenderCopy(renderer, Tile, NULL, &destination);
    SDL_RenderPresent(renderer);
}

void drawBackground(struct COLOR_MIX *frame_buffer){
    int i;
    int j;
    for (i=0; i<RADIUS*2; ++i){
        for (j=0; j<RADIUS*2; ++j){
            frame_buffer[j*RADIUS*2+i].RED = 0;
            frame_buffer[j*RADIUS*2+i].BLUE = 0;
            frame_buffer[j*RADIUS*2+i].GREEN = 0;
        }
    }
}

void drawBox(struct COLOR_MIX *frame_buffer, int length, int height){
    int i;
    int j;
    for (i=0; i<height; ++i){
        for (j=0; j<length; ++j){
            int x=i+RADIUS;
            int y=j+RADIUS;
            frame_buffer[y*RADIUS*2+x].RED = 0;
            frame_buffer[y*RADIUS*2+x].BLUE = 255;
            frame_buffer[y*RADIUS*2+x].GREEN = 0;
        }
    }
}

void drawSq(struct COLOR_MIX *frame_buffer, struct POINT_3D_FLOAT *p){
    int x;
    int y;
    for (x=-RADIUS; x<RADIUS; ++x){
        for(y=-RADIUS; y<RADIUS; ++y){
            struct POINT_3D_FLOAT s;
            s.X = (float)x;
            s.Y = (float)y;
            s.Z = 1.0f;
            if (inside_triangle(p, &s)){
                frame_buffer[(RADIUS - (int)y)*RADIUS*2+(RADIUS + (int)x)].RED = 0;
                frame_buffer[(RADIUS - (int)y)*RADIUS*2+(RADIUS + (int)x)].BLUE = 255;
                frame_buffer[(RADIUS - (int)y)*RADIUS*2+(RADIUS + (int)x)].GREEN = 0;
            }
        }
    }
}

void rotate_box(float x, float y, float a, float b, float angle){
    float p = a * cos(angle) - b * sin(angle);
    float q = a * sin(angle) + b * cos(angle);
    printf("%.3f %.3f\n", p + x, q + y);
}

void c_box(struct POINT_3D_FLOAT *p, float x, float y, float a, float b, float angle){
    float u = a * cos(angle) - b * sin(angle);
    float v = a * sin(angle) + b * cos(angle);
    p->X = u+x;
    p->Y = v+y;
    p->Z = 1.0f;
}

void draw(SDL_Renderer *renderer, struct COLOR_MIX *frame_buffer, float x, float y, float angle){
    struct POINT_3D_FLOAT p[3];
    //drawBackground(frame_buffer);
    c_box(p, x, y, -5, -5, angle);
    c_box(p+1, x, y, 5, 5, angle);
    c_box(p+2, x, y, -5, 5, angle);
    drawSq(frame_buffer, p);
    c_box(p, x, y, -5, -5, angle);
    c_box(p+1, x, y, 5, 5, angle);
    c_box(p+2, x, y, 5, -5, angle);
    drawSq(frame_buffer, p);
    draw_buffer(renderer, frame_buffer);
}

float tension(struct OBJECT *Space){
    float x;
    for (x=0.01f; x<1.0f; x+= 0.005){
        float val = -x * Space[0].LOCATION.X * Space[0].VELOCITY.X;
        if (fabs(val) < 0.001f){
            return x;
        }
    }
    return 0.0f;
}

void init(struct OBJECT *Space){
    Space[0].LOCATION.X = 0;
    Space[0].VELOCITY.X = 10;
    Space[0].LOCATION.Y = 50;
    Space[0].VELOCITY.Y = 0;
    Space[0].MASS = 1;
    Space[0].NET_FORCE.X = 0;
    Space[0].NET_FORCE.Y = 0;
}

int main(int argc, char *argv[]) {
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(RADIUS*2, RADIUS*2, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    struct COLOR_MIX *frame_buffer = malloc(RADIUS*RADIUS*4 * sizeof(struct COLOR_MIX));

    float time=0.0f;

    struct OBJECT Space[MAXIMUM_OBJECT_IN_SPACE];
    init(Space);
    float val = 0.01;
    float tmp = 0.0f;
    while (time < 100.0f) {
        Space[0].NET_FORCE.X = -val* Space[0].LOCATION.X;
        Space[0].NET_FORCE.Y = -val * Space[0].LOCATION.Y;
        printf("Tension - %.3f\n", sqrt(Space[0].NET_FORCE.X * Space[0].NET_FORCE.X + Space[0].NET_FORCE.Y * Space[0].NET_FORCE.Y));
        tmp = Space[0].NET_FORCE.X * Space[0].VELOCITY.X + Space[0].NET_FORCE.Y * Space[0].VELOCITY.Y;
        if (tmp < 0.0f){
            tmp = -tmp;
        }
        if (tmp > 1.0f){
            init(Space);
            val += 0.01f;
            time = 0.0f;
            drawBackground(frame_buffer);
            continue;
        }
        draw(renderer, frame_buffer, -Space[0].LOCATION.Y, -Space[0].LOCATION.X, 0.0f);
        f((void *)Space);
        SDL_Delay(1);
        time += DISCRETE_TIME_PACKET_VALUE;
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }

	free(frame_buffer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

