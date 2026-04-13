# ImmersioPlayer - High-Performance VR Media Player for Meta Quest 3

ImmersioPlayer is a standalone VR media player designed for the Meta Quest 3 hardware. It is built in Unreal Engine 5 using C++ and features a modular plugin architecture to handle media decoding, 180°/360° projection, UI, and network (SMB) streaming.

## Architecture

The project is divided into an entry module and four distinct plugins (`Plugins/` directory):

1. **CoreMedia:** Abstraction over the media player backend. Currently implements `UCoreMediaPlayerComponent` which wraps the native Unreal Media Framework (`AndroidMedia` for Quest). This abstraction (`IMediaBackend`) ensures libVLC or FFmpeg can be swapped in later without altering UI or Rendering code.
2. **VRRendering:** Handles mapping the video output to different physical surfaces. `AVRProjectionActor` switches between Flat, Curved, 180 Hemisphere, and 360 Sphere meshes, auto-detecting the format from the filename.
3. **UI:** Base classes for VR controls. `UVRMediaControlsWidget` connects play/pause/seek events to the `IMediaBackend`.
4. **Network:** Abstraction for SMB streaming. Currently implements `UMockSMBClient` for testing. A real backend (like libsmb2 or an HTTP bridge) can implement `ISMBClient` later.

The main project module (`ImmersioPlayer`) wires these together via the `AImmersioVRPawn`.

---

## Editor Setup Instructions (Manual Steps Required)

Because binary `.uasset` files cannot be generated purely from text, you must follow these steps in the Unreal Engine 5 Editor to complete the setup:

### 1. Enable Required Engine Plugins
- Go to **Edit > Plugins**. Ensure the following are enabled:
  - **OpenXR**
  - **Electra Player** (Highly recommended for performant streaming on Quest)
  - **Android Media Player**

### 2. Create the Rendering Materials
The C++ code expects a master material to be used for dynamic mapping.
1. Create a Material named `M_VRVideo_Master`.
2. Set its **Material Domain** to `Surface`, **Blend Mode** to `Opaque`, and **Shading Model** to `Unlit` (for performance and accurate colors).
3. Add a **Texture Sample Parameter 2D** node and name the parameter **`MediaTexture`**. Connect its RGB to `Emissive Color`.
4. To support SBS/TB 3D:
   - Add a **Scalar Parameter** named **`StereoMode`** (0=Monoscopic, 1=SBS, 2=TB).
   - Use a Custom Expression node or Material Function to alter the UV coordinates based on the current rendering eye (Left/Right) and the `StereoMode`.
5. Check **Two Sided** in the Material properties (important for spheres).

### 3. Create the Projection Meshes
Create or import the following simple static meshes:
- **Flat Screen** (A standard Plane)
- **Curved Screen** (A curved cylinder section)
- **180° Hemisphere** (A half-sphere, inverted normals)
- **360° Sphere** (A full sphere, inverted normals)

### 4. Setup the Projection Actor Blueprint
1. Create a Blueprint Class based on `AVRProjectionActor` (e.g., `BP_VRProjectionActor`).
2. Open it and assign the four meshes you created to the properties: `FlatMesh`, `CurvedMesh`, `HemisphereMesh`, `SphereMesh`.
3. Assign `M_VRVideo_Master` to the `BaseMaterial` property.

### 5. Setup the Media UI (UMG)
1. Create a Widget Blueprint (User Widget) based on `UVRMediaControlsWidget` (e.g., `WBP_MediaControls`).
2. Design your UI: Add Play, Pause, and a Seek slider.
3. In the widget's Graph, link the buttons' `OnClicked` events to the inherited C++ functions: `OnPlayClicked()`, `OnPauseClicked()`.
4. Link the slider's `OnValueChanged` event to `OnSeekScrubbed(Value)`.
5. Create an actor with a `WidgetComponent` in your VR space to display this UI.

### 6. Setup the VR Pawn Blueprint
1. Create a Blueprint Class based on `AImmersioVRPawn` (e.g., `BP_ImmersioVRPawn`).
2. In the properties, set `ProjectionActorClass` to the `BP_VRProjectionActor` you created in Step 4.
3. Configure the `VRCamera` height and tracking origin for Quest (Floor level).
4. Set this Pawn as the default Pawn class in your GameMode.

---

## Future SMB Integration

Currently, `UMockSMBClient` is used. For the Meta Quest (Android), a pure C++ SMB library (like `libsmb2`) can be compiled via Android NDK and linked as a third-party module.

Alternatively, a common approach for Unreal is to write a lightweight HTTP Server in C++ that streams data from the SMB library, providing a `http://127.0.0.1:...` URL to the Unreal `UMediaPlayer` for native buffering and hardware decoding support.
