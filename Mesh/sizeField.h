// Gmsh - Copyright (C) 1997-2021 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// issues on https://gitlab.onelab.info/gmsh/gmsh/issues.

#pragma once

class GModel;

int createSizeFieldFromExistingMesh(GModel *gm, bool computeCrosses);
