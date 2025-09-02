# 🎯 AI Tactical Battle Simulation - SecurityMap Graphics

## 🇮🇱 תיאור הפרויקט (Project Description)

פרויקט סיום לבינה מלאכותית המציג התנהגות חכמה של דמויות וירטואליות במשחק טקטי. הפרויקט מממש מערכת AI מתקדמת עם מפקד חכם, לוחמים אוטונומיים, חובש וחייל הספקה.

**Final AI project demonstrating intelligent behavior of virtual characters in a tactical game. The project implements an advanced AI system with a smart commander, autonomous warriors, medic, and supply soldier.**

## 🚀 תכונות עיקריות (Key Features)

### 🎖️ **מערכת מפקד חכם (Smart Commander System)**
- **תכנון אסטרטגי**: ניתוח מצב הקבוצה כל 2 שניות
- **הנפקת פקודות**: פקודות כלליות ללא פירוט
- **ניהול משאבים**: הקצאת חובש לחיילים פצועים, אספקה לחיילים עם תחמושת נמוכה
- **תכנון טקטי**: החלטות על התקפה או הגנה

### ⚔️ **לוחמים אוטונומיים (Autonomous Warriors)**
- **ניווט עצמאי**: אלגוריתם A* למציאת מסלול
- **בחירת נשק**: כדורים או רימונים לפי המצב
- **חיפוש כיסוי**: מציאת עמדות הגנה בטוחות
- **דיווח אוטומטי**: פציעה, תחמושת נמוכה, זיהוי אויב

### 🏥 **חובש חכם (Smart Medic)**
- **ניווט אוטומטי**: A* למחסן תרופות ולחייל פצוע
- **טיפול מלא**: החזרת חייל לבריאות 100%
- **תגובה מהירה**: תגובה אוטומטית לפקודות המפקד

### 📦 **חייל הספקה חכם (Smart Supply Soldier)**
- **אספקה אוטומטית**: מילוי תחמושת ורימונים
- **ניווט יעיל**: A* למחסן תחמושת ולחייל
- **ניהול מלאי**: מעקב אחר צרכי הקבוצה

## 🗺️ **מערכת המפה (Map System)**

### 🌍 **סוגי שטח (Terrain Types)**
- **סלעים (Stones)**: חוסמים תנועה, ירי ומבט
- **עצים (Trees)**: חוסמים ירי ומבט, תנועה אפשרית
- **מים (Water)**: חוסמים תנועה, ירי ומבט עוברים
- **מחסנים (Depots)**: תחמושת ותרופות לכל קבוצה

### 👁️ **מערכת נראות (Visibility System)**
- **נראות אישית**: כל חייל רואה חלק מהמפה
- **נראות משולבת**: המפקד רואה את שילוב הנראות של כל החיילים
- **קו ראייה**: אלגוריתם Bresenham לבדיקת חסימות

## 🧠 **אלגוריתמים (Algorithms)**

### 🛣️ **ניווט (Pathfinding)**
- **A***: ניווט יעיל עם התחשבות בסיכונים
- **A* מודע לסיכונים**: התחשבות במפת הבטיחות
- **BFS**: חיפוש נקודות בטוחות ועטיפה

### ⚡ **בינה מלאכותית (Artificial Intelligence)**
- **Finite State Machine (FSM)**: ניהול מצבים לכל דמות
- **תכנון אסטרטגי**: ניתוח מצב וקבלת החלטות
- **תיאום קבוצה**: תקשורת בין המפקד לחיילים

## 🎮 **הפעלת המשחק (Running the Game)**

### 📋 **דרישות מערכת (System Requirements)**
- Windows 10/11
- Visual Studio 2019/2022 עם C++ support
- OpenGL libraries (freeglut, glew)

### 🔧 **קומפילציה (Compilation)**

#### **אפשרות 1: Visual Studio**
```bash
# פתח את Graphics.vcxproj ב-Visual Studio
# בחר Debug או Release
# לחץ F5 להרצה
```

#### **אפשרות 2: Command Line**
```bash
# קומפילציה פשוטה
simple_compile.bat

# קומפילציה מלאה
compile.bat

# קומפילציה עם PowerShell
compile.ps1
```

### 🎯 **בקרות (Controls)**
- **T**: בדיקת נזק לחייל
- **A**: בדיקת תחמושת נמוכה
- **S**: הצג/הסתר מפת בטיחות
- **R**: בנייה מחדש של המפה והקבוצות
- **Q/ESC**: יציאה
- **עכבר שמאל**: זריקת רימון
- **עכבר ימין**: תפריט

## 📊 **מערכת הניצחון (Victory System)**

### 🏆 **תנאי ניצחון (Victory Conditions)**
- **השמדת מחסני האויב**: כל המחסנים (תחמושת + תרופות) חייבים להיהרס
- **מערכת בריאות**: חיילים עם פחות מ-25% בריאות לא יכולים לתפקד
- **מערכת תחמושת**: חיילים עם תחמושת נמוכה מבקשים אספקה

### 💀 **מערכת הלחימה (Combat System)**
- **ירי**: טווח 8 משבצות, 80% דיוק
- **רימונים**: טווח 6 משבצות, 90% דיוק
- **נזק**: כדורים ורימונים יכולים להרוס מחסנים

## 🏗️ **ארכיטקטורת הקוד (Code Architecture)**

### 📁 **מבנה הקבצים (File Structure)**
```
Graphics/
├── main.cpp              # Main game loop and OpenGL setup
├── CommanderNPC.*        # Commander AI with FSM
├── WarriorNPC.*          # Warrior AI with combat states
├── MedicNPC.*            # Medic AI with healing states
├── SupplierNPC.*         # Supply AI with supply states
├── Team.*                # Team coordination and management
├── AStar.*               # Pathfinding algorithms
├── Visibility.*          # Line of sight and visibility
├── State.*               # FSM state base classes
├── Definitions.h         # Game constants and enums
└── AI_IMPLEMENTATION_SUMMARY.md  # Detailed AI documentation
```

### 🔄 **מערכת המצבים (State System)**
כל דמות משתמשת ב-FSM עם מצבים שונים:

**מפקד (Commander)**:
- Planning → Commanding → Repositioning

**לוחם (Warrior)**:
- Idle → Moving → Attacking → Defending → Wounded → LowAmmo

**חובש (Medic)**:
- Idle → ToDepot → ToPatient → Treating

**חייל הספקה (Supplier)**:
- Idle → ToDepot → ToSoldier → Supplying

## 🎓 **למידה והתפתחות (Learning & Development)**

### 📚 **מושגי AI שנלמדו (AI Concepts Learned)**
1. **Finite State Machines**: ניהול מצבים מורכבים
2. **Pathfinding Algorithms**: A* ו-BFS לניווט
3. **Team Coordination**: תקשורת בין סוכנים
4. **Strategic Planning**: קבלת החלטות אסטרטגיות
5. **Tactical Behavior**: התנהגות טקטית עצמאית

### 🔬 **אזורי מחקר עתידיים (Future Research Areas)**
- **Machine Learning**: למידה מהתנהגות השחקן
- **Behavior Trees**: מערכות התנהגות מתקדמות
- **Multi-Agent Systems**: תיאום בין קבוצות גדולות
- **Procedural Generation**: יצירת מפות אוטומטית

## 🤝 **תרומה לפרויקט (Contributing)**

### 📝 **איך לתרום (How to Contribute)**
1. Fork את הפרויקט
2. צור branch חדש לתכונה
3. עשה commit לשינויים
4. פתח Pull Request

### 🐛 **דיווח באגים (Bug Reports)**
- תאר את הבעיה בפירוט
- צרף screenshots אם רלוונטי
- ציין גרסת מערכת הפעלה ומהדר

## 📄 **רישיון (License)**

פרויקט זה נוצר למטרות לימודיות. ניתן להשתמש בו בחופשיות ללימוד ופיתוח.

This project is created for educational purposes. It can be used freely for learning and development.

## 👨‍💻 **מפתחים (Developers)**

פרויקט סיום לבינה מלאכותית - אוניברסיטת בן-גוריון

Final AI Project - Ben-Gurion University

---

**🌟 בהצלחה! (Good Luck!) 🌟**
