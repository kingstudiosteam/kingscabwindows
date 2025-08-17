# The King's Cab - Custom Background Guide

## 🎨 Create Your Own Background

### **📐 Exact Specifications**
- **Width:** `840 pixels`
- **Height:** `620 pixels`
- **Format:** PNG (recommended for transparency support)
- **Filename:** `custom_background.png`
- **Location:** Place in `assets/` folder

---

## 🎯 **Layout Template**

```
┌─────────────────────────────────┐ ← 840px wide
│         HEADER ZONE             │ ← 80px height
│     (Logo area - keep clear)    │   (Your logo goes here)
├─────────────────────────────────┤
│                                 │
│      MAIN CONTENT AREA          │ ← 500px height
│                                 │
│  ┌─────────┐  ┌─────────┐      │ ← IR Slot positions
│  │ SLOT 1  │  │ SLOT 2  │      │   (keep background 
│  │   🎛️   │  │   🎛️   │      │    neutral here)
│  └─────────┘  └─────────┘      │
│                                 │
│  ┌─────────┐  ┌─────────┐      │
│  │ SLOT 3  │  │ SLOT 4  │      │
│  │   🎛️   │  │   🎛️   │      │
│  └─────────┘  └─────────┘      │
│                                 │
│  ┌─────────┐  ┌─────────┐      │
│  │ SLOT 5  │  │ SLOT 6  │      │
│  │   🎛️   │  │   🎛️   │      │
│  └─────────┘  └─────────┘      │
│                                 │
├─────────────────────────────────┤
│         FOOTER ZONE             │ ← 40px height
│   (Store link - keep readable)  │   (Gold text here)
└─────────────────────────────────┘
                                   ↑ 620px tall
```

---

## 🎨 **Design Guidelines**

### **✅ Recommended:**
- **Dark Backgrounds** → Better contrast for gold/white text
- **Gradients** → Add depth and luxury feel
- **Metallic Textures** → Brushed aluminum, steel, chrome
- **Subtle Patterns** → Carbon fiber, leather, wood grain
- **Kings Cab Branding** → Your logo, colors, themes

### **⚠️ Important Zones:**

#### **Header Zone (0-80px height):**
- Your logo will overlay this area
- Keep it clean or complementary
- Logo is positioned with 10px padding

#### **IR Slots Area (80-580px height):**
- 6 circular knobs with controls below
- Keep background neutral in these zones
- Slots are arranged in 2x3 grid
- Each slot is approximately 410x160px

#### **Footer Zone (580-620px height):**
- "Visit King Studios Store" link (gold text)
- Status display (right side)
- Ensure good text contrast

---

## 🔧 **How to Use**

1. **Create Your Design:**
   - Use Photoshop, GIMP, Figma, etc.
   - Design at exactly 840x620 pixels
   - Export as PNG for best quality

2. **Save the File:**
   ```
   /Users/justinmitchell/cursorfiles/cursorfiles/assets/custom_background.png
   ```

3. **Build & Test:**
   ```bash
   ./build_and_run.sh
   ```

4. **Debug:**
   - Check console for loading messages
   - If not loading, verify exact filename and location

---

## 💡 **Design Ideas**

### **Studio Themes:**
- Professional recording studio
- Vintage amp cabinet texture
- Modern metallic rack equipment
- Luxury studio control room

### **Brand Themes:**
- Kings Cab colors and styling
- Guitar cabinet wood grain
- Premium leather texture
- Gold/black luxury design

### **Technical Themes:**
- Circuit board patterns
- Waveform visualizations
- Professional EQ curves
- Audio spectrum displays

---

## 🎛️ **Fallback Behavior**

If no `custom_background.png` is found, the plugin will use the default luxury gradient background with:
- Multi-layer gradients
- Brushed metal texture
- Gold accent lines
- Professional borders

---

## 🔄 **Testing Your Design**

1. Place your PNG in `assets/custom_background.png`
2. Run `./build_and_run.sh`
3. Check console for loading confirmation
4. Test text readability
5. Verify knob visibility
6. Adjust design if needed

**Your custom background will give The King's Cab a completely unique, branded appearance that matches your vision!** 🎸👑

