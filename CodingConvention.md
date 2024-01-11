# Coding Convention

## Case
### General Rule
#### Use camelCase for variables.
```cpp
int32 pickedObjectIndex = -1;
```
#### Use PascalCase for classes, structs and functions.
```cpp
class Player;
class GUIContext;
struct RenderCommand;
void SetPosition(Float3::kZero);
PlayerID playerID = player.GetID();
```

### Acronyms
#### Use all UPPER CASE for acronyms
```cpp
uint32 GetHP() const;
uint32 HP = 10;
bool TestGUI();
void UpdateAI();
```

### Edge cases
#### Variables that start with acronyms
```cpp
struct GUITheme;
GUITheme aGUITheme;

class
{
    GUITheme    _GUITheme;
};

class Player
{
    uint32      _HP;
}

// How to bypass the problem
namesapce GUI
{
    struct Theme;
}
```

## Prefix
1. Fields start with an underscore.
```cpp
class Player
{
    uint32  _HP;
    String  _name;
};
```
2. (static) Constants uses prefix `k-`
```cpp
static constexpr uint32 kTessellation = 4;
static constexpr Float2 kZero = Float2(0.0f, 0.0f);
```

## Naming
#### Prefer `Compute-` to `Calculate-`
#### To access a field of a class, use `Set-`/`Get-`/`Access-` methods.
Although it seems meaningless using an `Access-` method when you could just make the field **public**,
it's possible that in some cases you need to modify the field, but in other cases, you don't want it to be modified.
In latter cases, it's better not to have a public field and you just call the `Get-` method.
```cpp
class Life
{
public:
    void            SetHP(const uint32);
    uint32          GetHP() const;

    const String&   GetName() const { return _name; }
    String&         AccessName() { return _name; }

private:
    uint32          _HP;
    String          _name;
};
```

#### Make/Generate/Create/Clone
##### Use `Make-` for simple objects or POD(Plain Old Data).
```cpp
Color MakeColor(const float r, const float g, const float b);
```

##### Use `Generate-` for classes with more things to be done.
```cpp
ControlID GenerateControlID(const char* const text, const ControlType type);
```

##### Use `Clone-` to create objects whose data is based on the existing ones.
```cpp
Color CloneAddRGB(const float r, const float g, const float b) const;
```

##### Do NOT use `Create-`, since this makes you confuse it with ctor.


#### Getter/Setter
##### Do not use `Get-` for bool getter. The condition statements get confusing.
<table style="border: 2px;">
<tr>
    <td><div style="text-align:center; color:#00AA00";>GOOD</div></td>
    <td><div style="text-align:center; color:#CC0000";>BAD</div></td>
</tr>
<tr>
<td>

```cpp
SetUseFullscreen(true);
if (UsesFullscreen()) // OR IsUsingFullscreen()
{
    // Do something.
}
```

</td>
<td>

```cpp
SetUseFullscreen(true);
if (GetUseFullscreen())
{
    // Do something.
}
```

</td>
</tr>
</table>


## Code style
#### Always use uniform initialization for constructor definitions. But when creating instances, prefer to use constructors explicitly.
```cpp
class Player
{
    Player(const float HP, const float MP)
        : _HP{ HP }
        , _MP{ MP }
    {
        __noop;
    }
    
private:
    float   _HP;
    float   _MP;
};

Player player = Player(100.0f, 200.0f);
```

#### If the parameter count for a function exceeds 3, consider grouping them into a class or struct
```cpp
// NOT SO GOOD
void HitTarget(const Float3& position, const float damage, const HitType hitType)
{
    // ...
}

// BETTER
struct HitTargetInput
{
    Float3  _position;
    float   _damage;
    HitType _hitType;
};
void HitTarget(const HitTargetInput& hitTargetInput)
{
    // ...
}
```


#### Never use C-style casting.
```cpp
// BAD
int intValue = (int)value;

// GOOD
int intValue = static_cast<int>(value);
```
#### Variable declaration must take a whole line.
```cpp
// BAD
float HPvalue = 100.0f, MPvalue = 100.0f;

// GOOD
float HPvalue = 100.0f;
float MPvalue = 100.0f;
```
#### Always use braces for single-statement blocks.
<table style="border: 2px;">
<tr>
    <td><div style="text-align:center; color:#00AA00";>GOOD</div></td>
    <td><div style="text-align:center; color:#CC0000";>BAD</div></td>
</tr>
<tr>
<td>

```cpp
if (IsValid() == false)
{
    return;
}
```

</td>
<td>

```cpp
if (IsValid() == false)
    return;
```

</td>
</tr>
</table>


#### Braces must take a whole line.
<table style="border: 2px;">
<tr>
    <td><div style="text-align:center; color:#00AA00";>GOOD</div></td>
    <td><div style="text-align:center; color:#CC0000";>BAD</div></td>
</tr>
<tr>
<td>

```cpp
if (IsValid() == false)
{
    return;
}
```

</td>
<td>

```cpp
if (IsValid() == false) {
    return;
}
```

</td>
</tr>
</table>


#### Prefer implicit `true` comparison for conditions.
<table style="border: 2px;">
<tr>
    <td><div style="text-align:center; color:#00AAAA";>PREFER</div></td>
    <td><div style="text-align:center; color:#000000";>OK</div></td>
</tr>
<tr>
<td>

```cpp
if (IsValid())
{
    return true;
}

if (IsValid() && IsVisible())
{
    return true;
}
```

</td>
<td>

```cpp
if (IsValid() == true)
{
    return true;
}
```

</td>
</tr>
</table>


#### Use explicit `false` comparison for conditions.
<table style="border: 2px;">
<tr>
    <td><div style="text-align:center; color:#00AA00";>GOOD</div></td>
    <td><div style="text-align:center; color:#CC0000";>BAD</div></td>
</tr>
<tr>
<td>

```cpp
if (IsValid() == false)
{
    return;
}
```

</td>
<td>

```cpp
if (!IsValid()) {
    return;
}
```

</td>
</tr>
</table>

