# Coding Convention

## Case
### General Rule
#### Use camel case for variables and functions.
```cpp
int32 pickedObjectIndex = -1;
void setPosition(Float3::kZero);
```
#### Use pascal case for classes and structs.
```cpp
class Player;
```

### Acronyms
#### Use all UPPER CASE for acronyms
```cpp
const uint32 getHP() const;
uint32 HP = 10;
const bool testGUI();
void updateAI();
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
#### Prefer `compute-` to `calculate-`
#### To access a field of a class, use `set-`/`get-`/`access-` methods.
Although it seems meaningless using an `access-` method when you could just make the field **public**,
it's possible that in some cases you need to modify the field, but in other cases, you don't want it to be modified.
In latter cases, it's better not to have a public field and you just call the `get-` method.
```cpp
class Life
{
public:
    void            setHP(const uint32);
    const uint32    getHP() const;

    const String&   getName() const { return _name; }
    String&         accessName() { return _name; }

private:
    uint32          _HP;
    String          _name;
};
```

#### make/generate/create/clone
##### Use `make-` for simple objects or POD(Plain Old Data).
```cpp
Color makeColor(const float r, const float g, const float b);
```

##### Use `generate-` for classes with more things to be done.
```cpp
ControlID generateControlID(const char* const text, const ControlType type);
```

##### Use `clone-` to create objects whose data is based on the existing ones.
```cpp
Color cloneAddRGB(const float r, const float g, const float b) const;
```

##### Do NOT use `create-`, since this makes you confuse it with ctor.

## Code style
#### Always use braces for single-statement blocks.
<table style="border: 2px;">
<tr>
    <td><div style="text-align:center; color:#00AA00";>GOOD</div></td>
    <td><div style="text-align:center; color:#CC0000";>BAD</div></td>
</tr>
<tr>
<td>

```cpp
if (isValid() == false)
{
    return;
}
```

</td>
<td>

```cpp
if (isValid() == false)
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
if (isValid() == false)
{
    return;
}
```

</td>
<td>

```cpp
if (isValid() == false) {
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
if (isValid())
{
    return true;
}
```

</td>
<td>

```cpp
if (isValid() == true)
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
if (isValid() == false)
{
    return;
}
```

</td>
<td>

```cpp
if (!isValid()) {
    return;
}
```

</td>
</tr>
</table>
