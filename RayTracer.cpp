#include <fstream>
#include "Object.hpp"
#include "Vector.hpp"
#include "Ray.hpp"
#include "Integrator.hpp"
#include "Color.hpp"
#include <thread>
#include <algorithm>

#define USEGL
#ifdef USEGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

constexpr int THREADS = 12;
std::vector<Sphere> spheres;
std::vector<Plane> planes;
std::vector<const Object*> objects;

struct Task {
    Ray ray;
    Vec3* pixel;

    Task() = default;
    Task(const Ray &ray, Vec3 * pixel_ptr) : ray(ray), pixel(pixel_ptr) {}
    Task(const Task &t) : ray(t.ray), pixel(t.pixel) {}
};


struct JobList {
    std::vector<Task> tasks;

    Task* getTask() {
        static const uint64_t taskCount = tasks.size();
        lock.lock();
        uint64_t n = index++;
        lock.unlock();
        if (n >= taskCount)
            return nullptr;
        return &tasks[n];
    }

    constexpr uint64_t getProgress() const { return index; }
    
private:
    std::mutex lock;
    uint64_t index{ 0 };
};

void render_thread(JobList* joblist)
{
    static const Float Inv_N = 1.0 / N;
    static const Float inv_pixel_size = 1.0 / sqrt(Float(W)*W + Float(H)*H);
    while (Task *task = joblist->getTask())
    {
        Vec3 &pixel = *(task->pixel);
        for (int n=0; n < N; n++) {
            const Vec3 h = task->ray.direction + random_hemi_vector(task->ray.direction) * inv_pixel_size;
            const Ray ray = Ray::NormalizedRay(task->ray.position, h);
            pixel += Li(objects, ray);
        }
        pixel = ACESFilm(pixel * Inv_N);
    }
}

int main() {
    planes.emplace_back(Vec3(0, 1, 0), -1, &SPECULAR_WHITE);

    //*
    for (int i=0; i < 100; i++) {
        Vec3 u(0,-1,0);
        while (u.y < 0)
            u = random_unit_vector();
        
        bool diffuse = (UniRand() < 0.5);
        spheres.emplace_back(
            Vec3(0,0,4) + u * 3, UniRand()*0.25 + 0.25,
            diffuse ? static_cast<const BxDF*>(&DIFFUSE_WHITE) : static_cast<const BxDF*>(&SPECULAR_WHITE)
        );
    }
    /*/
    spheres.emplace_back(Vec3(0,0,3), 1, &DIFFUSE_WHITE);
    //*/

    for (const Sphere& obj : spheres)
        objects.emplace_back(&obj);
    for (const Plane& obj : planes)
        objects.emplace_back(&obj);

    std::vector<Vec3> Pixels(W*H);
    JobList jobs;

    const Vec3 P(0, 0, -0.5);
    for (int i=0; i < H; i++) {
        for (int j=0; j < W; j++) {
            const Vec3 receiver((j-W*0.5+0.5)/H, (H*0.5-i-0.5)/H, 0);
            const Vec3 dir = (receiver-P).normalize();
            jobs.tasks.emplace_back(Ray(P, dir), &Pixels[i * W + j]);
        }
    }

    std::random_shuffle(jobs.tasks.begin(), jobs.tasks.end());

    #ifdef USEGL
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow((int)(1080.0/H*W), 1080, "RT", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewInit();

    glEnable(GL_TEXTURE_2D);

    GLuint texture;
    glGenTextures(1, &texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, W, H, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glfwSwapInterval(1);
    #endif

    std::thread threads[THREADS];

    auto t1 = std::chrono::high_resolution_clock::now();

    for (int thr=0; thr < THREADS; thr++) {
        threads[thr] = std::thread(render_thread, &jobs);
    }

    #ifdef USEGL
    double lastUpdate = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        double t = glfwGetTime();
        if (t-lastUpdate >= 0.5) {
            lastUpdate = t;
            int percProg = clamp(1.0 * jobs.getProgress() / W / H) * 100;
            glfwSetWindowTitle(window, ("RayTracer " + std::to_string(percProg)).c_str());
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W, H, GL_RGB, GL_FLOAT, Pixels.data());
        }
        glBegin(GL_QUADS);
        glTexCoord2i(0, 1);
        glVertex2i(-1, -1);

        glTexCoord2i(1, 1);
        glVertex2i(1, -1);

        glTexCoord2i(1, 0);
        glVertex2i(1, 1);

        glTexCoord2i(0, 0);
        glVertex2i(-1, 1);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteTextures(1, &texture);
    glfwTerminate();
    #endif

    for (int thr=0; thr < THREADS; thr++) {
        threads[thr].join();
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << std::endl << duration << "ms\n";


    std::ofstream f("raw.data", std::ios::binary);
    for (int i=0; i < H; i++) {
        for (int j=0; j < W; j++) {
            const Vec3 &c = Pixels[i * W + j];
            uint8_t r = static_cast<uint8_t>(clamp(c.r, 0, 1) * 255.0);
            uint8_t g = static_cast<uint8_t>(clamp(c.g, 0, 1) * 255.0);
            uint8_t b = static_cast<uint8_t>(clamp(c.b, 0, 1) * 255.0);

            f << r << g << b;
        }
    }

    return 0;
}