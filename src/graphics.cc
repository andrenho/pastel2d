#include "graphics.hh"

#include <SDL_image.h>
#include <SDL_ttf.h>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

using namespace std::string_literals;

namespace ps {

Graphics::Graphics(std::string const& window_title, int window_width, int window_height, SDL_WindowFlags flags)
    : window_title_(window_title)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    SDL_SetHint (SDL_HINT_RENDER_VSYNC, "1");

    window_ = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, flags);
    if (!window_)
        throw std::runtime_error("Error: SDL_CreateWindow(): "s + SDL_GetError());

    ren_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (!ren_)
        throw std::runtime_error("Error: SDL_CreateRenderer(): "s + SDL_GetError());
    res_.set_renderer(ren_);

    SDL_RendererInfo info;
    SDL_GetRendererInfo(ren_, &info);
    SDL_Log("Current SDL_Renderer: %s", info.name);

    init_imgui();
}

Graphics::~Graphics()
{
    finalize_imgui();

    res_.cleanup();

    if (ren_)
        SDL_DestroyRenderer(ren_);
    if (window_)
        SDL_DestroyWindow(window_);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void Graphics::step()
{
    // start frame
    auto new_frame = hr::now();
    ++frame_count_;
    ++frame_time_ += (new_frame - last_frame_);
    ++total_frames_;

    // get events
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        event(e);
        ImGui_ImplSDL2_ProcessEvent(&e);
    }

    // update
    update(new_frame - last_frame_);

    // render
    render();

    // show FPS
    if (frame_count_ > 20) {
        int fps = 1000 / (std::chrono::duration_cast<std::chrono::milliseconds>(frame_time_).count() / frame_count_);
        SDL_SetWindowTitle(window_, (window_title_ + " (" + std::to_string(fps) + " FPS)").c_str());
        frame_time_ = std::chrono::milliseconds(0);
        frame_count_ = 0;
    }

    last_frame_ = new_frame;
}

void Graphics::render() const
{
    // user functions
    Scene scene = create_scene();

    // background
    SDL_SetRenderDrawColor(ren_, scene.bg[0], scene.bg[1], scene.bg[2], SDL_ALPHA_OPAQUE);
    SDL_RenderClear(ren_);

    render_scene(scene);

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    draw_gui();
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), ren_);

    SDL_RenderPresent(ren_);
}

void Graphics::render_scene(Scene const& scene) const
{
    for (auto const& image: scene.images())
        render_image(image);
}

void Graphics::render_image(Scene::Image const& image) const
{
    SDL_Texture* texture = nullptr;
    SDL_Rect origin;

    auto resource = res().get(image.resource);

    if (auto tx = std::get_if<SDL_Texture*>(&resource)) {
        texture = *tx;
        origin.x = origin.y = 0;
        SDL_QueryTexture(texture, nullptr, nullptr, &origin.w, &origin.h);
    } else if (auto tile = std::get_if<Tile>(&resource)) {
        texture = tile->texture;
        origin = { tile->x, tile->y, tile->w, tile->h };
    } else {
        throw std::runtime_error("Invalid resource.");
    }

    float zoom = image.pen.zoom;
    SDL_Rect dest = {
        .x = (int) (image.x / image.pen.zoom),
        .y = (int) (image.y / image.pen.zoom),
        .w = origin.w,
        .h = origin.h
    };
    if (image.pen.full_screen == Pen::FullScreen::Stretch) {
        int scr_w, scr_h;
        SDL_GetWindowSize(window_, &scr_w, &scr_h);
        dest = { .x = 0, .y = 0, .w = scr_w, .h = scr_h };
        zoom = 1;
    }

    SDL_SetTextureAlphaMod(texture, 255 * image.pen.opacity);

    SDL_Point center = { dest.w / 2, dest.h / 2 };
    if (image.pen.rotation_center) {
        center.x = image.pen.rotation_center->first;
        center.y = image.pen.rotation_center->second;
    }

    SDL_RenderSetScale(ren_, zoom, zoom);
    SDL_RenderCopyEx(ren_, texture, &origin, &dest, image.pen.rotation, &center, SDL_FLIP_NONE);
    SDL_RenderSetScale(ren_, 1.f, 1.f);
}

void Graphics::init_imgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window_, ren_);
    ImGui_ImplSDLRenderer2_Init(ren_);
}

void Graphics::finalize_imgui()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

} // ps
