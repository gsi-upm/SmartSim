class GUIManager:
	def __init__(self):
		self.buttonList = []	
		self.sliderList = []		
		self.textList = []	
		self.chkBoxList = []
		self.comboList = []
		self.editBoxList = []
		self.winManager = WindowManager.getSingleton()	
		self.guiContext = System.getSingleton().getDefaultGUIContext()
		self.rootWindow = self.guiContext.getRootWindow()
		self.buttonWidth = 150
		self.buttonHeight = 25
		self.sliderWidth = 150
		self.sliderHeight = 10	
		self.staticTextWidth = 200
		self.staticTextHeight = 15	
		self.chkBoxWidth = 150
		self.chkBoxHeight = 25
		self.comboWidth = 150
		self.comboHeight = 100
		self.listBoxItem1 = ListboxTextItem('gdsajglkajglksag')
		self.listBoxItem2 = ListboxTextItem('aaaaaaaaaaaaasfsf')
		self.listBoxItem3 = ListboxTextItem('bbbbbbbbbdgfsfdsf')
		
		self.GUIRowHeight = 560		
		self.GUIRowWidth = 500
		
		self.GUISkinType = 'OgreTray/'
		
		self.ButtonType = self.GUISkinType+"Button"
		self.TextType = self.GUISkinType+"StaticText"
		self.SliderType = self.GUISkinType+"Slider"
		self.CheckBoxType = self.GUISkinType+"Checkbox"
		self.ComboBoxType = self.GUISkinType+"Combobox"
		self.EditBoxType = self.GUISkinType+"Editbox"
		'''
		self.ButtonType = "TaharezLook/Button"
		self.TextType = "TaharezLook/StaticText"
		self.SliderType = "TaharezLook/Slider"
		self.CheckBoxType = "TaharezLook/CheckBox"		
		
		self.ButtonType = "WindowsLook/Button"
		self.TextType = "WindowsLook/StaticText"
		self.SliderType = "WindowsLook/Slider"
		self.CheckBoxType = "WindowsLook/CheckBox"
		self.ComboBoxType = "WindowsLook/Combobox"
		self.EditBoxType = "WindowsLook/Editbox"
		'''
		
	def setWidgetPosition(self, widget, x,y):
		widget.setPosition(UVector2(UDim(0.0, x), UDim(0.0, y)))	
		
	def offsetWidgetPosition(self, widget, offsetX, offsetY):
		uv = widget.getPosition()
		x = uv.d_x.d_offset
		y = uv.d_y.d_offset
		widget.setPosition(UVector2(UDim(0.0, x+offsetX), UDim(0.0, y+offsetY)))	
	
	def getAllWidgetsHeight(self):
		height = len(self.buttonList)*(self.buttonHeight+5) + len(self.sliderList)*(self.sliderHeight+5) + len(self.textList)*(self.staticTextHeight+5) + len(self.chkBoxList)*(self.chkBoxHeight+5) + 10
		outVal = [height%self.GUIRowHeight, height/self.GUIRowHeight*self.GUIRowWidth]
		return outVal
	
	def createCheckBox(self, chkBoxName, chkBoxText, posX = -1, posY = -1, width = -1):
		winManager = WindowManager.getSingleton()	
		chkBox = winManager.createWindow(self.CheckBoxType, chkBoxName)
		chkBox.setUsingAutoRenderingSurface(False)
		chkBox.setClippedByParent(False)
		chkBox.setText(chkBoxText)				
		chkBox.setProperty("HoverTextColour","FFFF00")		
		boxWidth = self.chkBoxWidth
		if width != -1:
			boxWidth = width
		chkBox.setSize(USize(UDim(0.0, boxWidth), UDim(0.0, self.chkBoxHeight)))		
		if posX != -1 and posY != -1:
			self.setWidgetPosition(chkBox,posX,posY)
		else:
			[yoffset, xoffset] = self.getAllWidgetsHeight()
			xoffset += 15
			self.setWidgetPosition(chkBox,xoffset,yoffset)
		
		self.rootWindow.addChild(chkBox)				
		self.chkBoxList.append(chkBox)
		return chkBox	

	def createComboBox(self, comboName, comboTextList, posX = -1, posY = -1, width = -1):
		winManager = WindowManager.getSingleton()	
		combo = winManager.createWindow(self.ComboBoxType, comboName)	
		combo.setUsingAutoRenderingSurface(False)	
		combo.setClippedByParent(False)
		comboWidth = self.comboWidth
		if width != -1:
			comboWidth = width
		combo.setSize(USize(UDim(0.0, comboWidth), UDim(0.0, self.comboHeight)))		
		if posX != -1 and posY != -1:
			self.setWidgetPosition(combo,posX,posY)
		else:
			[yoffset, xoffset] = self.getAllWidgetsHeight()
			xoffset += 15
			self.setWidgetPosition(combo,xoffset,yoffset)	
				
		#for i in range(0,len(comboTextList)):
			#boxItem = ListboxTextItem(comboTextList[i])		
			# print boxItem
			#print combo
			#dropList = combo.getDropList()
			#print dropList
			#dropList.addItem(boxItem)
			#combo.clearAllSelections()
			#combo.addItem(boxItem)
		
		combo.addItem(self.listBoxItem1)
		combo.addItem(self.listBoxItem2)
		combo.addItem(self.listBoxItem3)
		
		self.rootWindow.addChild(combo)				
		self.comboList.append(combo)
		return combo	
		
	def createButton(self, btnName, btnText, posX = -1, posY = -1, width = -1):
		winManager = WindowManager.getSingleton()	
		button = winManager.createWindow(self.ButtonType, btnName)
		button.setUsingAutoRenderingSurface(False)
		button.setClippedByParent(False)
		button.setText(btnText)		
		button.setProperty("HoverTextColour","FFFF00")
		
		
		buttonWidth = self.buttonWidth
		if width != -1:
			buttonWidth = width
		button.setSize(USize(UDim(0.0, buttonWidth), UDim(0.0, self.buttonHeight)))		
		if posX != -1 and posY != -1:
			self.setWidgetPosition(button,posX,posY)
		else:
			[yoffset, xoffset] = self.getAllWidgetsHeight()
			xoffset += 15
			self.setWidgetPosition(button,xoffset,yoffset)	
		
		self.rootWindow.addChild(button)				
		self.buttonList.append(button)
		return button		
		
	def createEditBox(self, editBoxName, editBoxText, posX = -1, posY = -1, width = -1):
		winManager = WindowManager.getSingleton()	
		editBox = winManager.createWindow(self.EditBoxType, editBoxName)
		editBox.setUsingAutoRenderingSurface(False)
		editBox.setText(editBoxText)	
		editBox.setClippedByParent(False)
				
		editBoxWidth = self.buttonWidth
		if width != -1:
			editBoxWidth = width
		editBox.setSize(USize(UDim(0.0, editBoxWidth), UDim(0.0, self.buttonHeight)))		
		if posX != -1 and posY != -1:
			self.setWidgetPosition(editBox,posX,posY)
		else:
			[yoffset, xoffset] = self.getAllWidgetsHeight()
			xoffset += 15
			self.setWidgetPosition(editBox,xoffset,yoffset)	
		editBox.setReadOnly(False)
		editBox.setTextMasked(False)
		self.rootWindow.addChild(editBox)				
		self.editBoxList.append(editBox)
		return editBox	
		
	def createStaticText(self, textName, text, posX = -1, posY = -1, width = -1, height = -1):
		winManager = WindowManager.getSingleton()		
		staticText = winManager.createWindow(self.TextType, textName)		
		staticText.setUsingAutoRenderingSurface(False)
		staticText.setClippedByParent(False)
		staticText.setText(text)
		staticText.setSize(USize(UDim(0.0, self.staticTextWidth), UDim(0.0, self.staticTextHeight)))	
		#staticText.setProperty("TextColours", "FFFF00");
		staticText.setProperty("BackgroundEnabled", "false"); 
		staticText.setProperty("FrameEnabled", "false"); 	
		
		textWidth = self.staticTextWidth
		textHeight = self.staticTextHeight
		if width != -1:
			textWidth = width
		if height != -1:
			textHeight = height
			
		staticText.setSize(USize(UDim(0.0, textWidth), UDim(0.0, textHeight)))	
		if posX != -1 and posY != -1:
			self.setWidgetPosition(staticText,posX,posY)
		else:
			[yoffset, xoffset]= self.getAllWidgetsHeight()
			xoffset += 20			
			self.setWidgetPosition(staticText,xoffset,yoffset)			
		
		self.rootWindow.addChild(staticText)	
		self.textList.append(staticText)
		return staticText
		
		
	def createSlider(self, sliderName, max, defVal = 0.0, posX = -1, posY = -1, width = -1):
		winManager = WindowManager.getSingleton()		
		
		staticText = winManager.createWindow(self.TextType, sliderName+'Txt')		
		staticText.setText(sliderName)		
		staticText.setProperty("BackgroundEnabled", "false"); 
		staticText.setProperty("FrameEnabled", "false"); 			
		slider = winManager.createWindow(self.SliderType , sliderName)		
		slider.setText('testSliderText')
		slider.setProperty('VerticalSlider','False')
		#slider.setProperty("Font", "DejaVuSans-10");			
		slider.setMaxValue(max)		
		slider.setCurrentValue(defVal)		
		staticText.setUsingAutoRenderingSurface(False)
		staticText.setClippedByParent(False)
		slider.setUsingAutoRenderingSurface(False)
		slider.setClippedByParent(False)
		txtWidth = self.staticTextWidth
		sliderWidth = self.sliderWidth
		if width != -1:
			txtWidth = 	width/2	
			sliderWidth = width/2
		
		staticText.setSize(USize(UDim(0.0, txtWidth), UDim(0.0, self.staticTextHeight)))	
		slider.setSize(USize(UDim(0.0, sliderWidth), UDim(0.0, self.sliderHeight)))	
		if posX != -1 and posY != -1:
			self.setWidgetPosition(staticText,posX,posY)
			self.setWidgetPosition(slider,posX+txtWidth+5,posY)
		else:		
			[yoffset, xoffset]= self.getAllWidgetsHeight()
			#print 'slider yoffset = ' + str(yoffset)
			xoffset += 15				
			self.setWidgetPosition(staticText,xoffset,yoffset)			
			xoffset += 100 + 10
			self.setWidgetPosition(slider,xoffset,yoffset)
		
		self.rootWindow.addChild(staticText)		
		self.rootWindow.addChild(slider)	
		self.textList.append(staticText)		
		self.sliderList.append([staticText, slider])		
		return slider
	
		
#gui = GUIManager()
#gui.createStaticText('sliderText','text for slider bar')
#gui.createButton('button1','text for button 1')
#lider('slider1',0.5)