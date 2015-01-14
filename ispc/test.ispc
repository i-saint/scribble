
typedef float<3> vec3f;

static inline float dot3(vec3f a, vec3f b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}
static inline float length_sq3(vec3f v)
{
    return dot3(v, v);
}
static inline float length3(vec3f v)
{
    return sqrt(length_sq3(v));
}


struct Context
{
    float *pos_x;
    float *pos_y;
    float *pos_z;
    float *vel_x;
    float *vel_y;
    float *vel_z;
    int num_particles;
    float radius;
    float stiffness;
};


export void BruteForceCollision(uniform Context &context)
{
    const float radius = context.radius;
    const float radius2 = radius*2.0f;
    const float stiffness = context.stiffness;
    uniform int num_particles = context.num_particles;
    float *uniform pos_x = context.pos_x;
    float *uniform pos_y = context.pos_y;
    float *uniform pos_z = context.pos_z;
    float *uniform vel_x = context.vel_x;
    float *uniform vel_y = context.vel_y;
    float *uniform vel_z = context.vel_z;

    for(uniform int i=0; i<num_particles; ++i) {
        uniform vec3f pos1 = {pos_x[i], pos_y[i], pos_z[i]};
        vec3f accel = {0.0f, 0.0f, 0.0f};

        //foreach(n=0 ... num_particles) {
        for(uniform int nn=0; nn<num_particles; nn+=programCount) {
            int n = nn + programIndex;
            if(i==n) { continue; }

            vec3f pos2 = {pos_x[n], pos_y[n], pos_z[n]};
            vec3f diff = pos2 - pos1;
            float d = length3(diff);
            vec3f dir = diff / d;
            float p = min(d-radius2, 0.0f);
            accel += dir*p * stiffness;
        }

        uniform vec3f a = {reduce_add(accel.x), reduce_add(accel.y), reduce_add(accel.z)};
        vel_x[i] += a.x;
        vel_y[i] += a.y;
        vel_z[i] += a.z;
    }
}

