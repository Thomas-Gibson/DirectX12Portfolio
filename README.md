<table>
  <tr>
    <td>
      <img src="icon.ico" alt="DirectX 12" width="128" />
    </td>
    <td>
      <h1>DirectX12Portfolio</h1>
      <p>
        My collection of Direct3D12 projects. Features an application framework that hot loads DLLs to quickly switch between, run, and view projects.
      </p>
    </td>
  </tr>
</table>

<br>

# How to use
<details> <summary> Prerequisites </summary>
    <ul>
        <li>Have the latest version of Visual Studio 2026 installed/updated with the C++ desktop development workload.
        <li>Make sure you have the latest Windows SDK installed. This can be obtained by updating or installing Visual Studio.</li>
        <li>Have CMake installed on your system.</li>
    </ul>
</details>

<details> <summary> Run Instructions </summary>
    <ol>
        <li>Clone the git repository: https://github.com/Thomas-Gibson/DirectX12Portfolio.git </li>
        <li>Right-click on your copy and under "Show more options" select "Open with Visual Studio." </li>
        <li>Make sure the build target is set to the top level CMAKELISTS.txt. </li>
        <li>Press f5 to build and run.</li>
        <li>It will take some time to build as it brings over dependencies such as Agility SDK, ASSIMP, DirectXTK12 so grab some coffee ☕.</li>
    </ol>
</details>


# Projects
* [Hello Triangle](./Projects/Dx12Triangle/) - Draws a simple rainbow triangle.
* [Hello Texture](./Projects/Dx12Texture/) - Shows texture loading and drawing.
* [Directional Lighting](./Projects/Dx12Lighting/) - Shows directional lighting applied to a 3D model.
