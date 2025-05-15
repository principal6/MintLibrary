# MintLibrary
```mermaid
classDiagram
    MintCommon <|-- MintContainer
    MintCommon <|-- MintMath

    MintContainer <|-- MintAudio
    MintContainer <|-- MintPlatform
    MintContainer <|-- MintRendering
    MintContainer <|-- MintReflection
    MintContainer <|-- MintLanguage

    MintLanguage <|-- MintRendering

    MintRendering <|-- MintRendering

    MintRendering <|-- MintPhysics
    MintMath <|-- MintPhysics

    MintRendering <|-- MintGUI
    MintPhysics <|-- MintGUI

    MintAudio <|-- MintApp
    MintPlatform <|-- MintApp
    MintReflection <|-- MintApp
    MintGUI <|-- MintApp

    MintApp <|-- MintGame

    MintGame <|-- MintLibrary

    class MintCommon{
        IntegerTypes
        Assert/Log
        Path
        Max() / Min() / Clamp()
    }
    class MintContainer{
        Algorithm
        Hash
        Color
        ID
        OwnPtr / SharedPtr
    }
    class MintPlatform{
        File IO
        XML Parser
        InputContext
    }
    class MintMath{
        Rect
        Transform
        Geometry
        Easing
        ...
    }
    class MintRendering{
        GraphicsDevice
        Renderer
        ShapeGenerator
        ImageLoader
        SpriteAnimation
        ...
    }
    class MintRendering{
        SpriteRenderer
        InstantRenderer
        MeshGenerator
        SplineGenerator
        ...
    }
    class MintPhysics{
        CollisionShape
        Intersection
        PhysicsWorld
        ...
    }
    class MintApp{
        SceneObjectPool
        SceneObject
        App
        ...
    }
```