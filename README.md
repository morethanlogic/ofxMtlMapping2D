ofxMtlMapping2D
=====================================

Introduction
------------
C++ openFrameworks addon to achieve 2D projection-mapping of textures to physical objects. Each instance of ofxMtlMapping2D has its own FBO to draw content into. Each created shape has an output polygon used for mapping, and an input polygon defining the texture coordinates to be used.

**Output mode**  
![output mode](http://www.morethanlogic.com/ofxMtl/ofxMtlMapping2D/ofxMtlMapping2D_output.png)

**Input mode**  
![input mode](http://www.morethanlogic.com/ofxMtl/ofxMtlMapping2D/ofxMtlMapping2D_input.png)

**Settings**  
![settings](http://www.morethanlogic.com/ofxMtl/ofxMtlMapping2D/ofxMtlMapping2D_settings.png)

Some projects using this addon...
------------
... or variations of it.
<table style="border-style: hidden; background:#FFFFFF">
<tr>
<td style="border-style: hidden; background:#FFFFFF"><a href="http://www.smallfly.com/2012/08/echos/"><img src="http://www.morethanlogic.com/ofxMtl/ofxMtlMapping2D/ofxMtlMapping2D_Echos__.jpg" alt="Echos"></a></td>
<td style="border-style: hidden; background:#FFFFFF"><a href="http://www.smallfly.com/2011/12/barcode/"><img src="http://www.morethanlogic.com/ofxMtl/ofxMtlMapping2D/ofxMtlMapping2D_Barcode.jpg" alt="Barcode"></a></td>
</tr>
<tr>
<td style="border-style: hidden; background:#FFFFFF"><a href="http://www.smallfly.com/2011/06/306/"><img src="http://www.morethanlogic.com/ofxMtl/ofxMtlMapping2D/ofxMtlMapping2D_DesignOpenHouse.jpg" alt="Open House"></a></td>
<td style="border-style: hidden; background:#FFFFFF"><a href="http://www.smallfly.com/2011/06/sam-roberts-band-i-feel-you/"><img src="http://www.morethanlogic.com/ofxMtl/ofxMtlMapping2D/ofxMtlMapping2D_SamRobertBand.jpg" alt="Sam Robert's Band"></a></td>
</tr>
<tr>
<td style="border-style: hidden; background:#FFFFFF"><a href="http://www.smallfly.com/2013/11/tedxmontreal/"><img src="http://www.morethanlogic.com/ofxMtl/ofxMtlMapping2D/ofxMtlMapping2D_TEDxMTL.jpg" alt="TEDxMTL"></a></td>
</tr>
</table>

Licence
-------
The code in this repository is available under the [MIT License](https://en.wikipedia.org/wiki/MIT_License)  
Copyright (c) 2014 Hugues Bruyère, Elie Zananiri, [More Than Logic](http://www.morethanlogic.com) | [Dpt.](http://dpt.co)

Installation
------------
Copy to your openFrameworks/addons folder. Some examples are included.

Dependencies
------------
* [For now this specific branch of ofxMSAInteractiveObject](https://github.com/smallfly/ofxMSAInteractiveObject/tree/feature/coord_transformation)
* [ofxUI](https://github.com/rezaali/ofxUI)
* ofxXmlSettings
* homography.h by Arturo Castro (included in this repo)

Optional add-on(s). See the 'ofxMtlMapping2DDefines.h' header file.
* [ofxDetectDisplays](https://github.com/morethanlogic/ofxDetectDisplays)
* [ofxSyphon](https://github.com/astellato/ofxSyphon)

Compatibility
------------
OF 0.8.3