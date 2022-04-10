#include <iostream>
#include <vector>
#include "GL/glew.h"
#include "GLFW/glfw3.h"



// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include "implot/implot.h"
#include "implot/implot_internal.h"

#include "nfd/nfd.hpp"

#include "cif_transform.h"
#include "cif.h"


using namespace row;


// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif


static void glfw_error_callback(int error, const char* description)
{
   fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


void addAWindow(bool& show_demo_window, bool& show_another_window, ImVec4 clear_color) {
   static float f = 0.0f;
   static int counter = 0;

   ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

   ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
   ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
   ImGui::Checkbox("Another Window", &show_another_window);

   ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
   ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

   if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
      counter++;
   ImGui::SameLine();
   ImGui::Text("counter = %d", counter);

   ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
   ImGui::End();

}



//*********************************************************************************
//
//  combo box
//
//*********************************************************************************


void dropdown(const std::string& title, const std::vector<std::string>& values, const size_t& preview_idx, size_t& current_index ) {

   static ImGuiComboFlags flags = 0;
   if (values.size() == 0) {
      if (ImGui::BeginCombo(title.c_str(), "No values", flags))
         ImGui::EndCombo();
   }
   else if (ImGui::BeginCombo(title.c_str(), values[preview_idx].c_str(), flags))
   {
      for (int n = 0; n < values.size(); n++)
      {
         const bool is_selected = (current_index == n);
         if (ImGui::Selectable(values[n].c_str(), is_selected))
            current_index = n;

         // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
         if (is_selected)
            ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
   }
}

//*********************************************************************************
//
//  axis titles
//
//*********************************************************************************

//need to change this function so it doesn't construct the map every frame'
/*
std::string x_axis_title(const std::string& x_axis, std::string lam) {
   if (lam.size() == 0)
      lam = "(Wavelength unknown)";
   else
      lam = "(lam = " + lam + " Å)";

   std::unordered_map<std::string, std::string> X_AXIS_TITLES =
   { "_pd_meas_2theta_scan": f"\u00B0 2\u03b8 {wavelength}",
     "_pd_proc_2theta_corrected" : f"\u00B0 2\u03b8 corrected {wavelength}",
     "_pd_meas_time_of_flight" : "Time of flight (\u00b5s)",
     "_pd_meas_position" : "Position (mm)",
     "_pd_proc_energy_incident" : "Incident energy (eV)",
     "_pd_proc_wavelength" : "Incident wavelength (\u212b)",
     "_pd_proc_d_spacing" : "d spacing (\u212b)",
     "_pd_proc_recip_len_Q" : "q (1/\u212b)",
     "d" : "d spacing (\u212b)",
     "q" : "q (1/\u212b)" };

   return X_AXIS_TITLES[x_axis];
}
*/


//*********************************************************************************
//
//  Single plot controls
//
//*********************************************************************************

size_t single_pattern_index{ 0 };
size_t single_x_axis_index{ 0 };
size_t single_y_obs_index{ 0 };
size_t single_y_calc_index{ 0 };
size_t single_y_bkg_index{ 0 };




void single_plot_controls(const row::cif::transform::File& file) {

   const std::vector<std::string>& pattern_block_ids = file.pattern_block_ids;
   const row::cif::transform::Pattern& pattern = file.patterns[single_pattern_index];

   ImGui::Begin("Single plot controls");                          

   std::vector<std::string> empty{};
   dropdown("Data", pattern_block_ids, single_pattern_index, single_pattern_index);
   ImGui::Separator();
   dropdown("X axis",  pattern.x_axis.tags, single_x_axis_index, single_x_axis_index);
   dropdown("Y(obs)", pattern.y_obs.tags, single_y_obs_index, single_y_obs_index);
   dropdown("Y(calc)", pattern.y_calc.tags, single_y_calc_index, single_y_calc_index);
   dropdown("Y(bkg)", pattern.y_bkg.tags, single_y_bkg_index, single_y_bkg_index);

   //ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
   //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
   //ImGui::Checkbox("Another Window", &show_another_window);

   //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
   //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

   //if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
   //   counter++;
   //ImGui::SameLine();
   //ImGui::Text("counter = %d", counter);

   ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
   ImGui::End();

}



//for my legend popup
static float  frequency = 0.1f;
static float  amplitude = 0.5f;
static ImVec4 colour = ImVec4(1, 0, 1, 1);
static bool   line = true;
static float  thickness = 1;
static bool   markers = false;

void MakeMainMenuBar(GLFWwindow* window) {
   if (ImGui::BeginMainMenuBar())
   {
      if (ImGui::BeginMenu("File"))
      {
         if (ImGui::MenuItem("Open...")) { std::cout << "This is where you call native file dialog to get a file." << std::endl; }
         if (ImGui::MenuItem("Save")) { std::cout << "do something with the menu item." << std::endl; }
         ImGui::Separator();
         if (ImGui::MenuItem("Close")) { glfwSetWindowShouldClose(window, true); }
         ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Edit"))
      {
         if (ImGui::MenuItem("Cut")) { std::cout << "Something got cut." << std::endl; }
         if (ImGui::MenuItem("Copy")) { std::cout << "Something got copied."; }
         if (ImGui::MenuItem("Paste")) { std::cout << "PASTE!" << std::endl; }
         ImGui::EndMenu();
      }
      ImGui::Separator();
      if (ImGui::Button("Save")) { std::cout << "Then save something!" << std::endl; }


      ImGui::EndMainMenuBar();
   }
}


template<typename T, typename A>
void print(const std::vector<T, A>& v, const std::string& header = "") {
   std::cout << "\n-- " << header << " --" << std::endl;
   for (size_t i{ 0 }; i < v.size(); ++i)
      std::cout << i << ":\t" << v[i] << std::endl;
}

int main(int, char**)
{

   ///*********************************************
   // open file dialog
   // initialize NFD
   NFD::Guard nfdGuard;

   // auto-freeing memory
   NFD::UniquePath outPath;

   // prepare filters for the dialog
   nfdfilteritem_t filterItem[2] = { {"Source code", "c,cpp,cc"}, {"Headers", "h,hpp"} };

   // show the dialog
   nfdresult_t result = NFD::OpenDialog(outPath, filterItem, 2);
   if (result == NFD_OKAY) {
      std::cout << "Success!" << std::endl << outPath.get() << std::endl;
   }
   else if (result == NFD_CANCEL) {
      std::cout << "User pressed cancel." << std::endl;
   }
   else {
      std::cout << "Error: " << NFD::GetError() << std::endl;
   }
   //******************************************************************

   //create some data to plot

   cif::Cif cif{};
   try {
      cif = cif::read_file("C:\\data\\test.cif");
   }
   //catch (pegtl::parse_error& e) {
   catch (std::runtime_error e) {
      std::cerr << e.what() << std::endl;
     return 1; // a parse error is fatal, so return here to terminate program.
   }

   cif::transform::File file(cif);


   // Setup window
   glfwSetErrorCallback(glfw_error_callback);
   if (!glfwInit())
      return 1;

   // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
   const char* glsl_version = "#version 100";
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
   glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
   const char* glsl_version = "#version 150";
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
   const char* glsl_version = "#version 130";
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
   //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
   //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
   GLFWwindow* window = glfwCreateWindow(1280, 720, "pdqCIFviewer", NULL, NULL);
   if (window == nullptr)
      return 1;
   glfwMakeContextCurrent(window);
   glfwSwapInterval(1); // Enable vsync

   // Setup Dear ImGui context
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImPlot::CreateContext(); //and set up ImPlot
   ImGuiIO& io = ImGui::GetIO(); (void)io;
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
   io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
   io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
   //io.ConfigViewportsNoAutoMerge = true;
   //io.ConfigViewportsNoTaskBarIcon = true;
   
   // Setup Dear ImGui style
   //ImGui::StyleColorsDark();
   //ImGui::StyleColorsClassic();
   ImGui::StyleColorsLight();

   // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
   ImGuiStyle& style = ImGui::GetStyle();
   if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
   {
      style.WindowRounding = 0.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
   }

   // Setup Platform/Renderer backends
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init(glsl_version);

   // Load Fonts
   // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
   // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
   // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
   // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
   // - Read 'docs/FONTS.md' for more instructions and details.
   // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
  // io.Fonts->AddFontDefault();
   io.Fonts->AddFontFromFileTTF("src/vendor/imgui/misc/fonts/Roboto-Medium.ttf", 16.0f);
   io.Fonts->AddFontFromFileTTF("src/vendor/imgui/misc/fonts/Cousine-Regular.ttf", 15.0f);
   io.Fonts->AddFontFromFileTTF("src/vendor/imgui/misc/fonts/DroidSans.ttf", 16.0f);
   io.Fonts->AddFontFromFileTTF("src/vendor/imgui/misc/fonts/ProggyTiny.ttf", 10.0f);
   //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
   //IM_ASSERT(font != NULL);

   // Our state
   bool show_demo_window = true;
   bool show_another_window = false;
   ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



   // Event loop
   while (!glfwWindowShouldClose(window))
   {
      // Poll and handle events (inputs, window resize, etc.)
      // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
      // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
      // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
      // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
      glfwPollEvents();

      // Start the Dear ImGui frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
      ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()); //makes the main window dockable with the child windows
      

      // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
      if (show_demo_window)
         ImGui::ShowDemoWindow(&show_demo_window);

      // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
      //addAWindow(show_demo_window, show_another_window, clear_color);
      //{
      //   static float f = 0.0f;
      //   static int counter = 0;

      //   ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

      //   ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
      //   ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
      //   ImGui::Checkbox("Another Window", &show_another_window);

      //   ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
      //   ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

      //   if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
      //      counter++;
      //   ImGui::SameLine();
      //   ImGui::Text("counter = %d", counter);

      //   ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      //   ImGui::End();
      //}

      // 3. Show another simple window.
      if (show_another_window)
      {
         ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
         ImGui::Text("Hello from another window!");
         if (ImGui::Button("Close Me"))
            show_another_window = false;
         ImGui::End();
      }
      

      MakeMainMenuBar(window);


      const std::vector<double>& x_values = file.patterns[single_pattern_index].x_axis[file.patterns[single_pattern_index].x_axis.tags[single_x_axis_index]].values;
      const std::vector<double>& y_values = file.patterns[single_pattern_index].y_obs[file.patterns[single_pattern_index].y_obs.tags[single_y_obs_index]].values;

      single_plot_controls(file);




#if 1
      //ImPlot::ShowDemoWindow();
      ImGui::Begin("pdqCIFv");
      if (ImPlot::BeginPlot("Diffraction data", ImVec2(-1, -1), ImPlotFlags_AntiAliased)) {
         //for setup, see implot.h from line 544
         ImPlot::SetupAxes("X-axis label", "Y-axis label");
         ImPlot::SetupLegend(ImPlotLocation_NorthEast);
         ImPlot::SetupMouseText(ImPlotLocation_SouthEast);
         ImPlot::SetupFinish();
     
         if (markers) ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
         ImPlot::SetNextLineStyle(colour, thickness);
         //ImPlot::PlotLine("Ian's sample", x_values, y_values, 1767);

         /*if(x_values!=nullptr && y_values)*/   ImPlot::PlotLine("Ian's sample", x_values.data(), y_values.data(), static_cast<int>(x_values.size()));
          



         if (ImPlot::BeginLegendPopup("Right Click Me")) {
            ImGui::ColorEdit3("Colour", &colour.x);
            ImGui::SliderFloat("Thickness", &thickness, 0, 5);
            ImGui::Checkbox("Markers", &markers);
            ImPlot::EndLegendPopup();
         }
         ImPlot::EndPlot();
      }
      ImGui::End();
#endif




      // Rendering
      ImGui::Render();
      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT);
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      // Update and Render additional Platform Windows
      // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
      //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
      if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
         GLFWwindow* backup_current_context = glfwGetCurrentContext();
         ImGui::UpdatePlatformWindows();
         ImGui::RenderPlatformWindowsDefault();
         glfwMakeContextCurrent(backup_current_context);
      }

      glfwSwapBuffers(window);
   }//end of event loop

   // Cleanup
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImPlot::DestroyContext();
   ImGui::DestroyContext();



   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}
