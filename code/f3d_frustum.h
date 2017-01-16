// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_FRUSTUM_H)

internal b32
FrustumCheckSphere(glm::mat4 MVP, glm::vec3 Center, GLfloat radius)
{
    GLfloat *MVPMatrix = glm::value_ptr(MVP);
    GLfloat xPos = Center.x;
    GLfloat yPos = Center.y;
    GLfloat zPos = Center.z;
    
    enum term { A = 0, B, C, D };
    
    GLfloat leftPlane[4];
    leftPlane[A] = MVPMatrix[3]  + MVPMatrix[0];
    leftPlane[B] = MVPMatrix[7]  + MVPMatrix[4];
    leftPlane[C] = MVPMatrix[11] + MVPMatrix[8];
    leftPlane[D] = MVPMatrix[15] + MVPMatrix[12];
    
    // Normalise the plane
    GLfloat length = sqrtf(leftPlane[A] * leftPlane[A] + leftPlane[B] * leftPlane[B] + leftPlane[C] * leftPlane[C]);
    leftPlane[A] /= length;
    leftPlane[B] /= length;
    leftPlane[C] /= length;
    leftPlane[D] /= length;
    
    // Check the point's location with respect to the left plane of our viewing frustrum
    GLfloat distance = leftPlane[A] * xPos + leftPlane[B] * yPos + leftPlane[C] * zPos + leftPlane[D];
    if (distance <= -radius)
    {
        return false; // Bounding sphere is completely outside the left plane
    }
    
    // Check the point's location with respect to the right plane of our viewing frustum
    GLfloat rightPlane[4];
    rightPlane[A] = MVPMatrix[3]  - MVPMatrix[0];
    rightPlane[B] = MVPMatrix[7]  - MVPMatrix[4];
    rightPlane[C] = MVPMatrix[11] - MVPMatrix[8];
    rightPlane[D] = MVPMatrix[15] - MVPMatrix[12];
    
    // Normalise the plane
    length = sqrtf(rightPlane[A] * rightPlane[A] + rightPlane[B] * rightPlane[B] + rightPlane[C] * rightPlane[C]);
    rightPlane[A] /= length;
    rightPlane[B] /= length;
    rightPlane[C] /= length;
    rightPlane[D] /= length;
    
    distance = rightPlane[A] * xPos + rightPlane[B] * yPos + rightPlane[C] * zPos + rightPlane[D];
    if (distance <= -radius)
    {
        return false; // Bounding sphere is completely outside the right plane
    }
    
    // Check the point's location with respect to the bottom plane of our viewing frustum
    GLfloat bottomPlane[4];
    bottomPlane[A] = MVPMatrix[3]  + MVPMatrix[1];
    bottomPlane[B] = MVPMatrix[7]  + MVPMatrix[5];
    bottomPlane[C] = MVPMatrix[11] + MVPMatrix[9];
    bottomPlane[D] = MVPMatrix[15] + MVPMatrix[13];
    
    // Normalise the plane
    length = sqrtf(bottomPlane[A] * bottomPlane[A] + bottomPlane[B] * bottomPlane[B] + bottomPlane[C] * bottomPlane[C]);
    bottomPlane[A] /= length;
    bottomPlane[B] /= length;
    bottomPlane[C] /= length;
    bottomPlane[D] /= length;
    
    distance = bottomPlane[A] * xPos + bottomPlane[B] * yPos + bottomPlane[C] * zPos + bottomPlane[D];
    if (distance <= -radius)
    {
        return false; // Bounding sphere is completely outside the bottom plane
    }
    
    // Check the point's location with respect to the top plane of our viewing frustrum
    GLfloat topPlane[4];
    topPlane[A] = MVPMatrix[3]  - MVPMatrix[1];
    topPlane[B] = MVPMatrix[7]  - MVPMatrix[5];
    topPlane[C] = MVPMatrix[11] - MVPMatrix[9];
    topPlane[D] = MVPMatrix[15] - MVPMatrix[13];
    
    // Normalise the plane
    length = sqrtf(topPlane[A] * topPlane[A] + topPlane[B] * topPlane[B] + topPlane[C] * topPlane[C]);
    topPlane[A] /= length;
    topPlane[B] /= length;
    topPlane[C] /= length;
    topPlane[D] /= length;
    
    distance = topPlane[A] * xPos + topPlane[B] * yPos + topPlane[C] * zPos + topPlane[D];
    if (distance <= -radius)
    {
        return false; // Bounding sphere is completely outside the top plane
    }
    
    // Check the point's location with respect to the near plane of our viewing frustum
    GLfloat nearPlane[4];
    nearPlane[A] = MVPMatrix[3]  + MVPMatrix[2];
    nearPlane[B] = MVPMatrix[7]  + MVPMatrix[6];
    nearPlane[C] = MVPMatrix[11] + MVPMatrix[10];
    nearPlane[D] = MVPMatrix[15] + MVPMatrix[14];
    
    // Normalise the plane
    length = sqrtf(nearPlane[A] * nearPlane[A] + nearPlane[B] * nearPlane[B] + nearPlane[C] * nearPlane[C]);
    nearPlane[A] /= length;
    nearPlane[B] /= length;
    nearPlane[C] /= length;
    nearPlane[D] /= length;
    
    distance = nearPlane[A] * xPos + nearPlane[B] * yPos + nearPlane[C] * zPos + nearPlane[D];
    if (distance <= -radius)
    {
        return false; // Bounding sphere is completely outside the near plane
    }
    
    // Check the point's location with respect to the far plane of our viewing frustum
    GLfloat farPlane[4];
    farPlane[A] = MVPMatrix[3]  - MVPMatrix[2];
    farPlane[B] = MVPMatrix[7]  - MVPMatrix[6];
    farPlane[C] = MVPMatrix[11] - MVPMatrix[10];
    farPlane[D] = MVPMatrix[15] - MVPMatrix[14];
    
    // Normalise the plane
    length = sqrtf(farPlane[A] * farPlane[A] + farPlane[B] * farPlane[B] + farPlane[C] * farPlane[C]);
    farPlane[A] /= length;
    farPlane[B] /= length;
    farPlane[C] /= length;
    farPlane[D] /= length;
    
    distance = farPlane[A] * xPos + farPlane[B] * yPos + farPlane[C] * zPos + farPlane[D];
    if (distance <= -radius)
    {
        return false; // Bounding sphere is completely outside the far plane
    }
    
    // If we got here, then the bounding sphere is within at least all six sides of the view frustum, so it's visible and we should draw it!
    return true;
}

#define F3D_FRUSTUM_H
#endif