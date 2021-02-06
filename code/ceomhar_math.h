/* date = September 29th 2020 0:36 am */
#pragma warning(push)
#pragma warning(disable: 4505)

#define F32_ROUND(f) (f32)((u32)(f + 0.5f))

#define CLAMP_MAX(n,max) ((n > max) ? max : n)
#define CLAMP_MIN(n,min) ((n < min) ? min : n)

// TODO(Cian): move this to math stuff later
union V2 {
    struct {
        f32 x,y;
    };
    struct {
        f32 width, height;
    };
    struct {
        u32 start,end;
    };
    struct {
        f32 min, max;
    };
    struct {
        f32 size, strictness;
    };
    f32 elements[2];
};

union V3 {
    struct{
        f32 x,y,z;
    };
    f32 elements[3];
};

union V4 {
    struct
    {
        f32 x;
        f32 y;
        union
        {
            struct
            {
                f32 z;
                
                union
                {
                    f32 w;
                    f32 radius;
                };
            };
            struct
            {
                f32 width;
                f32 height;
            };
        };
    };
    
    struct
    {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };
    
    f32 elements[4];
};

internal V2 v2(f32 x, f32 y) {
    V2 res = {
        x ,y
    };
    return res;
}

internal V3 v3(f32 x, f32 y, f32 z) {
    V3 res = {
        x ,y, z
    };
    return res;
}

internal V4 v4(f32 x, f32 y, f32 z, f32 w) {
    V4 res = {
        x ,y,z,w
    };
    return res;
}
#pragma warning(pop)