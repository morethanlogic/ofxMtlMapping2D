ofxMtlMapping2D
=====================================

Introduction
------------
C++ openFrameworks addon to achieve 2D projection-mapping of textures to physical objects. Each instance of ofxMtlMapping2D has its own FBO to draw content into. Each created shape has an output polygon used for mapping, and an input polygon defining the texture coordinates to be used.

**Output mode**  
![output mode](http://www.morethanlogic.com/ofxMtl/ofxMtlMapping2D/ofxMtlMapping2D_output.png)

**Input mode**  
![input mode](http://www.morethanlogic.com/ofxMtl/ofxMtlMapping2D/ofxMtlMapping2D_input.png)

Some projects using this addon...
------------
... or variations of it.
<table style="border-style: hidden; background:#FFFFFF">
<tr>
<td style="border-style: hidden; background:#FFFFFF"><a href="http://www.smallfly.com/2012/08/echos/"><img src="http://www.morethanlogic.com/ofxMtl/ofxMtlMapping2D/ofxMtlMapping2D_Echos__.jpg" alt="Echos"></a></td>
<td style="border-style: hidden; background:#FFFFFF"><a href="http://www.smallfly.com/2011/12/barcode/"><img src="http://www.morethanlogic.com/ofxMtl/ofxMtlMapping2D/ofxMtlMapping2D_Barcode.jpg" alt="Barcode"></a></td>
</tr>
<tr>
<td style="border-style: hidden; background:#FFFFFF"><a href="http://www.smallfly.com/2011/06/306/"><img src="http://www.morethanlogic.com/ofxMtl/ofxMtlMapping2D/ofxMtlMapping2D_DesignOpenHouse.jpg" alt="Open house"></a></td>
<td style="border-style: hidden; background:#FFFFFF"><a href="http://www.smallfly.com/2011/06/sam-roberts-band-i-feel-you/"><img src="http://www.morethanlogic.com/ofxMtl/ofxMtlMapping2D/ofxMtlMapping2D_SamRobertBand_.jpg" alt="Sam Robert's Band"></a></td>
</tr>
</table>

Licence
-------
The code in this repository is available under the [MIT License](https://en.wikipedia.org/wiki/MIT_License)  
Copyright (c) 2013 Hugues Bruyère, Elie Zananiri, [www.morethanlogic.com](http://www.morethanlogic.com) | [www.departement.ca](http://www.departement.ca)    
More Than Logic

Installation
------------
Copy to your openFrameworks/addons folder.

Dependencies
------------
* [ofxMSAInteractiveObject](https://github.com/memo/ofxMSAInteractiveObject)
* [ofxUI](https://github.com/rezaali/ofxUI)
* ofxXmlSettings
* homography.h by Arturo Castro (included in this repo)

Compatibility
------------
OF080