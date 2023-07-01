#include "rman.h"
#include "common.h"
#include "displaydriver.h"
#include <RixPredefinedStrings.hpp>
#include <RixSceneGraph.h>
#include <stdio.h>
#include <QMatrix>

namespace rsg = rman::scenegraph;

rsg::Scene* g_scene = nullptr;
rsg::Camera* g_camera = nullptr;

void createScene(RixSGManager* sgmngr, stats::Session& statsSession) {
    // Create the scene
    RtParamList config;
    RtParamList renderConfig;
    g_scene = sgmngr->CreateScene(config, renderConfig, statsSession);

    // Set scene options
    RtParamList options;
    int32_t format[2] = {RENDERWIDTH, RENDERHEIGHT};
    options.SetString(Rix::k_bucket_order, RtUString("circle"));
    options.SetIntegerArray(Rix::k_Ri_FormatResolution, format, 2);
    options.SetFloat(Rix::k_Ri_FormatPixelAspectRatio, 1.0f);
    g_scene->SetOptions(options);

    // Create a simple checker bxdf
    rsg::Material* material = g_scene->CreateMaterial(US_NULL);
    {
        rsg::Shader manifold(rsg::ShaderType::k_Pattern, RtUString("PxrManifold2D"),
                             RtUString("manifold"));
        manifold.params.SetFloat(RtUString("scaleS"), 4);
        manifold.params.SetFloat(RtUString("scaleT"), 2);
        rsg::Shader checker(rsg::ShaderType::k_Pattern, RtUString("PxrChecker"),
                            RtUString("checker"));
        checker.params.SetColor(RtUString("colorA"), RtColorRGB(1, 1, 0));
        checker.params.SetColor(RtUString("colorB"), RtColorRGB(0, 1, 1));
        checker.params.SetStructReference(RtUString("manifold"), RtUString("manifold:result"));
        rsg::Shader diffuse(rsg::ShaderType::k_Bxdf, RtUString("PxrDiffuse"), RtUString("diffuse"));
        diffuse.params.SetColorReference(RtUString("diffuseColor"), RtUString("checker:resultRGB"));
        rsg::Shader const bxdf[3] = {manifold, checker, diffuse};
        material->SetBxdf(3, bxdf);
    }

    // Create a sphere
    rsg::Quadric* sphere = g_scene->CreateQuadric(RtUString("sphere"));
    {
        sphere->SetMaterial(material);
        RtMatrix4x4 transform{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
        transform.Translate(0, 0.5, 0);
        transform.Scale(0.5, 0.5, 0.5);
        transform.Rotate(-90, 1, 0, 0);
        sphere->SetTransform(transform);
        g_scene->Root()->AddChild(sphere);
    }

    // Create render camera and parent under a group
//    g_cameraGroup = g_scene->CreateGroup(RtUString("g_cameraGroup"));
//    g_scene->Root()->AddChild(g_cameraGroup);
    g_camera = g_scene->CreateCamera(RtUString("camera"));
    {
        rsg::Shader proj(rsg::ShaderType::k_Projection, RtUString("PxrPerspective"),
                         RtUString("proj"));
        proj.params.SetFloat(Rix::k_fov, VIEW_FOV);
        g_camera->SetProjection(1, &proj);
        RtMatrix4x4 transform{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
        transform.Translate(INITIAL_CAM_POS);
        transform.Inverse(&transform);
        transform.Scale(1.0f, 1.0f, -1.0f);
        g_camera->SetTransform(transform);
        g_camera->SetRenderable(true);
        g_scene->Root()->AddChild(g_camera);
    }

    // Create an output display driver
    rsg::Shader display(rsg::ShaderType::k_Display, RtUString("looklabDisplay"), RtUString(""));
    //    rsg::Shader display(rsg::ShaderType::k_Display, RtUString("tiff"),
    //    RtUString("rmanTest.tif"));
    g_camera->SetDisplay(1, &display);

    // Render the scene blocking
    //        g_scene->Render("prman -blocking");
    // Render the scene live and asynchronous
    g_scene->Render("prman -live");
}

void rmanSetCamXform(const QMatrix4x4& xformMat) {
    {
        rsg::Scene::ScopedEdit edit(g_scene);
        RtMatrix4x4 transform;
        memcpy(transform.m, xformMat.inverted().data(), 16*sizeof(float));
        transform.Scale(1.0f, 1.0f, -1.0f);  // Z axis is different for openGL cam
        g_camera->SetTransform(transform);
    }
}

void startRender() {
    printf("Starting Render.\n");
    registerDisplayDriver();

    // Must be created before PRManBegin
    std::string sessionName("SG Simple Edit");
    stats::Session& statsSession = stats::AddSession(sessionName);

    // Wrap the RenderMan part of the process in PRManBegin/End
    RixContext* ctx = RixGetContext();
    RixRiCtl* rictl = (RixRiCtl*)ctx->GetRixInterface(k_RixRiCtl);
    rictl->PRManBegin(0, nullptr);

    Ri* rixRi = rictl->GetRiCtx();
    rixRi->ErrorHandler(RiErrorPrint);

    RixSGManager* sgmngr = (RixSGManager*)ctx->GetRixInterface(k_RixSGManager);

    createScene(sgmngr, statsSession);
}

void StopRender() {

    //    g_scene->Stop();

    //    sgmngr->DeleteScene(g_scene);
    //    rictl->PRManEnd();
}
