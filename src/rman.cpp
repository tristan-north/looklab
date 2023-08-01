#include "rman.h"
#include "common.h"
#include "displaydriver.h"
#include "geo.h"
#include <stdlib.h>
#include <QMatrix>
#include <RixPredefinedStrings.hpp>
#include <RixSceneGraph.h>
#include <stdio.h>

typedef const RtPoint3 pFloat3[8];
namespace rsg = rman::scenegraph;

rsg::Scene* g_scene = nullptr;
rsg::Camera* g_camera = nullptr;
rsg::Material* g_material = nullptr;

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

    rsg::Shader integrator(rsg::ShaderType::k_Integrator, RtUString("PxrPathTracer"),
                           RtUString("integrator"));
    g_scene->SetIntegrator(1, &integrator);
    // Create a simple checker bxdf
    //    rsg::Material* material = g_scene->CreateMaterial(US_NULL);
    //    {
    //        rsg::Shader manifold(rsg::ShaderType::k_Pattern, RtUString("PxrManifold2D"),
    //                             RtUString("manifold"));
    //        manifold.params.SetFloat(RtUString("scaleS"), 4);
    //        manifold.params.SetFloat(RtUString("scaleT"), 2);
    //        rsg::Shader checker(rsg::ShaderType::k_Pattern, RtUString("PxrChecker"),
    //                            RtUString("checker"));
    //        checker.params.SetColor(RtUString("colorA"), RtColorRGB(1, 1, 0));
    //        checker.params.SetColor(RtUString("colorB"), RtColorRGB(0, 1, 1));
    //        checker.params.SetStructReference(RtUString("manifold"),
    //        RtUString("manifold:result")); rsg::Shader diffuse(rsg::ShaderType::k_Bxdf,
    //        RtUString("PxrDiffuse"), RtUString("diffuse"));
    //        diffuse.params.SetColorReference(RtUString("diffuseColor"),
    //        RtUString("checker:resultRGB")); rsg::Shader const bxdf[3] = {manifold, checker,
    //        diffuse}; material->SetBxdf(3, bxdf);
    //    }

    g_material = g_scene->CreateMaterial(US_NULL);
    {
        rsg::Shader diffuse(rsg::ShaderType::k_Bxdf, RtUString("PxrDiffuse"), RtUString("diffuse"));
        diffuse.params.SetColor(RtUString("diffuseColor"), RtColorRGB(1, 0, 0));
        rsg::Shader const bxdf[] = {diffuse};
        g_material->SetBxdf(1, bxdf);
    }

    // LIGHTS
    {
        rsg::AnalyticLight* light = g_scene->CreateAnalyticLight(RtUString("rectLight"));
        rsg::Shader lightShader(rsg::ShaderType::k_Light, RtUString("PxrRectLight"), RtUString("rectLight"));
        lightShader.params.SetFloat(RtUString("intensity"), 25.8f);
        light->SetLight(1, &lightShader);
        RtMatrix4x4 transform{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
        transform.Translate(0, 3.5, 0);
        transform.Rotate(90, 1, 0, 0);
        transform.Scale(1, 1, 1);
        light->SetTransform(transform);
        g_scene->Root()->AddChild(light);
    }
    {
        rsg::AnalyticLight* light = g_scene->CreateAnalyticLight(RtUString("domeLight"));
        rsg::Shader lightShader(rsg::ShaderType::k_Light, RtUString("PxrDomeLight"), RtUString("domeLight"));
        lightShader.params.SetFloat(RtUString("intensity"), 0.3f);
        light->SetLight(1, &lightShader);
        g_scene->Root()->AddChild(light);
        RtParamList parmList;
        parmList.SetInteger(RtUString("visibility:camera"), 0);
        light->SetAttributes(parmList);
    }
    // Create a sphere
    //    rsg::Quadric* sphere = g_scene->CreateQuadric(RtUString("sphere"));
    //    {
    //        sphere->SetMaterial(material);
    //        RtMatrix4x4 transform{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    //        transform.Translate(0, 0.5, 0);
    //        transform.Scale(0.5, 0.5, 0.5);
    //        transform.Rotate(-90, 1, 0, 0);
    //        sphere->SetTransform(transform);
    //        g_scene->Root()->AddChild(sphere);
    //    }

    /*
        // Create box
        {
            // There's an array element for each face which is the number of verts of that face.
            int32_t const boxNVertices[2] = {4, 4};
            // Each element of the array is an index to the positions array
            int32_t const boxVertices[8] = {0, 1, 2, 3, 4, 5, 6, 7};
            pFloat3 shortBoxPoints = {
                {1.0f, 0.0f, -1.0f},   {-1.0f, 0.0f, -1.0f},  {-1.0f, 0.0f, 1.0f},
                {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, -1.0f},   {-1.0f, 1.0f, -1.0f},
                {-1.0f, 1.0f, 1.0f},  {1.0f, 1.0f, 1.0f},
            };
            rsg::Mesh* shortBox = g_scene->CreateMesh(RtUString("shortBox"));
            // Define args: num polys, num vec3 positions, num elements in index array
            shortBox->Define(2, 8, 8);
            RtPrimVarList primvars = shortBox->GetPrimVars();
            primvars.SetPointDetail(Rix::k_P, shortBoxPoints, RtDetailType::k_vertex);
            primvars.SetIntegerDetail(Rix::k_Ri_nvertices, boxNVertices, RtDetailType::k_uniform);
            primvars.SetIntegerDetail(Rix::k_Ri_vertices, boxVertices, RtDetailType::k_facevarying);
            shortBox->SetPrimVars(primvars);
            shortBox->SetMaterial(material);
            g_scene->Root()->AddChild(shortBox);
        }
    */
    {
        int numFaces = geo::getNumIndices() / 3;
        int32_t* numVerticesPerFace = (int32_t*)malloc(sizeof(int32_t) * numFaces);
        for (int i = 0; i < numFaces; ++i) {
            numVerticesPerFace[i] = 3;
        }

        rsg::Mesh* mesh = g_scene->CreateMesh(RtUString("mesh"));
        mesh->SetMaterial(g_material);
        mesh->Define(numFaces, geo::getNumPoints(), geo::getNumIndices());
        RtPrimVarList primvars = mesh->GetPrimVars();
        primvars.SetPointDetail(Rix::k_P, (RtFloat3*)geo::getPositions(), RtDetailType::k_vertex);
        primvars.SetNormalDetail(Rix::k_N, (RtNormal3*)geo::getNormals(), RtDetailType::k_vertex);
        primvars.SetIntegerDetail(Rix::k_Ri_nvertices, numVerticesPerFace, RtDetailType::k_uniform);
        primvars.SetIntegerDetail(Rix::k_Ri_vertices, (int32_t*)geo::getIndices(),
                                  RtDetailType::k_facevarying);
        mesh->SetPrimVars(primvars);
        g_scene->Root()->AddChild(mesh);
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
        memcpy(transform.m, xformMat.inverted().data(), 16 * sizeof(float));
        transform.Scale(1.0f, 1.0f, -1.0f); // Z axis is different for openGL cam
        g_camera->SetTransform(transform);
    }
}

void rmanSetColorParam(char* paramName, float Cx, float Cy, float Cz) {
    {
        rsg::Scene::ScopedEdit edit(g_scene);
        rsg::Shader pxrSurf(rsg::ShaderType::k_Bxdf, RtUString("LamaDiffuse"), RtUString("pixSurf"));
        pxrSurf.params.SetColor(RtUString(paramName), RtColorRGB(Cx, Cy, Cz));
        rsg::Shader const bxdf[] = {pxrSurf};
        g_material->SetBxdf(1, bxdf);
    }

    free(paramName);
    paramName = nullptr;
    printf("rmanSetColorParam\n");
}

void rmanSetFloatParam(char* paramName, float x) {
    {
        rsg::Scene::ScopedEdit edit(g_scene);
        rsg::Shader pxrSurf(rsg::ShaderType::k_Bxdf, RtUString("LamaDiffuse"), RtUString("pixSurf"));
        pxrSurf.params.SetFloat(RtUString(paramName), x);
        rsg::Shader const bxdf[] = {pxrSurf};
        g_material->SetBxdf(1, bxdf);
    }

    free(paramName);
    paramName = nullptr;
    printf("rmanSetFloatParam\n");
}

void rmanSetStringParam(char* paramName, char* stringValue) {
    {
        rsg::Scene::ScopedEdit edit(g_scene);
        rsg::Shader pxrSurf(rsg::ShaderType::k_Bxdf, RtUString("LamaDiffuse"), RtUString("pixSurf"));
        pxrSurf.params.SetString(RtUString(paramName), RtUString(stringValue));
        rsg::Shader const bxdf[] = {pxrSurf};
        g_material->SetBxdf(1, bxdf);
    }
    free(paramName);
    free(stringValue);
}

void rmanSetIntParam(char* paramName, bool boolValue) {
    {
        rsg::Scene::ScopedEdit edit(g_scene);
        rsg::Shader pxrSurf(rsg::ShaderType::k_Bxdf, RtUString("LamaDiffuse"), RtUString("pixSurf"));
        pxrSurf.params.SetInteger(RtUString(paramName), RtUInt64(boolValue));
        rsg::Shader const bxdf[] = {pxrSurf};
        g_material->SetBxdf(1, bxdf);
    }
    free(paramName);
}

void startRender() {
    printf("Starting Render.\n");
    registerDisplayDriver();

    // Must be created before PRManBegin
    std::string sessionName("Looklab Stats Session");
    stats::Session& statsSession = stats::AddSession(sessionName);

    // Wrap the RenderMan part of the process in PRManBegin/End
    RixContext* ctx = RixGetContext();
    RixRiCtl* rictl = (RixRiCtl*)ctx->GetRixInterface(k_RixRiCtl);

    const int numArgs = 5;
    char* args[numArgs];

    args[0] = _strdup("");
    args[1] = _strdup("-loglevel");
    args[2] = _strdup("4");
    args[3] = _strdup("-statsconfig");
    args[4] = _strdup("stats_disable.ini");

    rictl->PRManBegin(numArgs, args);

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
