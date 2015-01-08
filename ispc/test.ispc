
struct Particle
{
    float<3> position;
    float<3> velocity;
};

export void UpdateParticles(Particle particles[], uniform int num_particles)
{
    for(int i=0; i<num_particles; ++i) {
        particles[i].position += particles[i].velocity;
    }
}
