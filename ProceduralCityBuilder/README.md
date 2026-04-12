# Procedural City Builder (Unreal Engine 5 Plugin)

AI-Assisted Procedural City & World Builder optimized for performance.

## Installation

1. Open your Unreal Engine 5 project directory.
2. If it doesn't already exist, create a folder named `Plugins` in the root of your project directory (next to your `.uproject` file).
3. Copy the entire `ProceduralCityBuilder` folder from this repository into the `Plugins` folder.
4. Right-click your `.uproject` file and select **Generate Visual Studio project files** (or your preferred IDE).
5. Open the project in your IDE and compile, or simply launch the Unreal Engine editor and allow it to compile the new plugin.
6. Once the editor is open, go to **Edit > Plugins**, search for "Procedural City Builder", and ensure it is enabled.

## Usage

1. Open your Unreal Engine level.
2. In the Place Actors panel, search for **City Generator**.
3. Drag and drop the `City Generator` actor into your level.
4. With the actor selected, look at the **Details** panel.
5. Under the **City Generation | Components** category, you will see the HISM (Hierarchical Instanced Static Mesh) components.
   * Expand each HISM component (e.g., `HISM_BaseFloors`) and assign a placeholder `Static Mesh` in its properties so you can see the generated results.
6. Under the **City Generation** categories (Layout, Density, Buildings), you can adjust parameters like:
   * **Grid Size X/Y**: The overall dimensions of the city grid.
   * **Cell Size**: The physical size of one grid block.
   * **Min/Max Floors**: The randomized height of the buildings.
   * **Random Seed**: Change this to get a completely different city layout.
7. Click the **Generate City** button in the Details panel to instantly spawn the procedural layout!
8. Click **Clear City** to remove the generated meshes.

*Note: You can later replace the basic Static Meshes on the HISM components with your own high-quality modular asset packs (walls, roofs, floors, etc.).*
