/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved
 
VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
 
All data and information contained in or disclosed by this document are
 confidential and proprietary information of VisualOn, and all rights
 therein are expressly reserved. By accepting this material, the
 recipient agrees that this material and the information contained
 therein are held in confidence and in trust. The material may only be
 used and/or disclosed as authorized in a license agreement controlling
 such use and disclosure.
 ************************************************************************/

/************************************************************************
 * @file voSubTitleManager.java
 * Manage subtitle functions.
 *
 * 
 *
 *
 * @author  Li Mingbo
 * @date    2012-2012 
 ************************************************************************/

package com.visualon.OSMPSubTitle;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.BitmapFactory.Options;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PaintFlagsDrawFilter;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.Parcel;
import android.text.Layout;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.view.ViewParent;
import android.view.WindowManager;
import android.view.animation.AlphaAnimation;
import android.view.animation.TranslateAnimation;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.util.HashMap;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.apache.http.util.ByteArrayBuffer;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import com.visualon.OSMPUtils.voLog;
import com.visualon.OSMPUtils.voOSPreviewSubtitleInfo;
import com.visualon.OSMPUtils.voOSType;
import com.visualon.OSMPUtils.voOSType.VOOSMP_IMAGE_TYPE;


/**
 * OSMP+ closed caption manager class.
 * 
 * 
 * 
 */

public class voSubTitleManager {
	private static final String TAG = "@@@ClosedCaptionManager.java";
	private static final int KEY_START_TIME 				= 1;
	private static final int KEY_DURATION 				= 2;
	private static final int KEY_STRUCT_RECT 			= 3;
	private static final int KEY_RECT_BORDER_TYPE 			= 4;
	private static final int KEY_RECT_BORDER_COLOR 			= 5;
	private static final int KEY_RECT_FILL_COLOR 			= 6;
	private static final int KEY_RECT_Z_ORDER	 		= 7;
	private static final int KEY_DISPLAY_EFFECTTYPE			= 8;
	private static final int KEY_DISPLAY_EFFECTDIRECTION		= 9;
	private static final int KEY_DISPLAY_EFFECTSPEED			= 10;
	private static final int KEY_TEXT_SIZE				= 11;
	private static final int KEY_TEXT_STRING				= 12;
	private static final int KEY_FONT_INFO_SIZE			= 13;
	private static final int KEY_FONT_INFO_STYLE			= 14;
	private static final int KEY_FONT_INFO_COLOR			= 15;
	private static final int KEY_FONT_EFFECT_TEXTTAG			= 16;
	private static final int KEY_FONT_EFFECT_ITALIC			= 17;
	private static final int KEY_FONT_EFFECT_UNDERLINE		= 18;
	private static final int KEY_FONT_EFFECT_EDGETYPE		= 19;
	private static final int KEY_FONT_EFFECT_OFFSET			= 20;
	private static final int KEY_FONT_EFFECT_EDGECOLOR		= 21;
	private static final int KEY_TEXT_ROW_DESCRIPTOR_HORI		= 22;
	private static final int KEY_TEXT_ROW_DESCRIPTOR_VERT		= 23;
	private static final int KEY_TEXT_ROW_DESCRIPTOR_PRINT_DIRECTION	= 24;
	private static final int KEY_TEXT_DISPLAY_DESCRIPTOR_WRAP	= 25;
	private static final int KEY_TEXT_DISPLAY_DESCRIPTOR_SCROLL_DIRECTION = 26;
	private static final int KEY_IMAGE_SIZE				= 27;
	private static final int KEY_IMAGE_DATA			 	= 28;
	private static final int KEY_IMAGE_WIDTH            = 29;
	private static final int KEY_IMAGE_HEIGHT			= 30;
	private static final int KEY_IMAGE_TYPE             = 31;

	private static final int KEY_SUBTITLE_INFO			= 100;
	private static final int KEY_RECT_INFO				= 101;
	private static final int KEY_DISPLAY_INFO			= 102;
	private static final int KEY_TEXT_ROW_INFO			= 103;
	private static final int KEY_TEXT_INFO				= 104;
	private static final int KEY_IMAGE_INFO				= 105;
	private static final int KEY_STRING_INFO			= 106;
	private static final int KEY_FONT_INFO				= 107;
	private static final int KEY_FONT_EFFECT			= 108;
	private static final int KEY_TEXT_ROW_DESCRIPTOR	= 109;
	private static final int KEY_TEXT_DISPLAY_DESCRIPTOR		= 110;
	private static final int KEY_IMAGE_DISPLAY_DESCRIPTOR		= 111;

	private Parcel 							mParcel = null;
	private voSubtitleInfo 					mSubtitleInfo = null;
	private ArrayList<voSubtitleInfo>				mSubtitleInfoArray = new ArrayList<voSubtitleInfo>();
	private Context  						mainActivity = null;
	private ViewGroup 						rlMain = null;
	private RelativeLayout 					llWindow1 = null;
	private int 							height = 0;
	private int 							width = 0;
	private float 							xyRate = 1.33f;
	private float 							density = 1.0f;
	private boolean							mShow = true;
	protected voSubTitleFormatSettingImpl						settings = null;
	private TextOutLinesView				textViewOfRows = null;
//	private voThumbnailPlayList				thumbnailPlayList = null;
	
	//for checkxyRate
	private int 							heightOld = 0;
	private int 							widthOld = 0;
	private View							vwSurface = null;
	private boolean							mIsPreview = false;
	
	//for event voOSType.VOOSMP_CB_Metadata_Arrive
	private boolean 						metadata_Arrive = false;
	
	private Handler m_handlerEvent = new Handler(Looper.myLooper() != null? Looper.myLooper():Looper.getMainLooper()) {
	      public void handleMessage(Message msg) {
	    	  checkViewShowStatus(0);
	      }
	};
	
    /**
     * 
	 * Construct function
	*/
	public voSubTitleManager() {
		super();
	}
	
	private static Typeface fromID(int id){
		Typeface fontTf = Typeface.create(Typeface.DEFAULT, 0);
//		if(style == VOOSMP_Monospaced_with_serifs)
//			fontName = "Courier";
//		if(style == VOOSMP_FontStyle_Default_Proportionally_spaced_with_serifs)
//			fontName = "Times New Roman";
//		if(style == VOOSMP_FontStyle_Default_Monospaced_without_serifs)
//			fontName = "Helvetica";
//		if(style == VOOSMP_FontStyle_Default_Proportionally_spaced_without_serifs)
//			fontName = "Arial";
//		if(style == VOOSMP_FontStyle_Default_Casual)
//			fontName = "Dom";
//		if(style == VOOSMP_FontStyle_Default_Cursive)
//			fontName = "Coronet";
//		if(style == VOOSMP_FontStyle_Default_Small_capitals)
//			fontName = "Gothic";
		int type = 0;
		switch(id){
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Default:
			break;
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Courier:
		case voSubTitleFormatSetting.VOOSMP_Monospaced_with_serifs:
			fontTf = Typeface.create("Courier", type);
			break;
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Times_New_Roman:
			fontTf = Typeface.create("Times New Roman", type);
			break;
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Helvetica:
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Default_Monospaced_without_serifs:
			fontTf = Typeface.create("Helvetica", type);
			break;
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Arial:
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Default_Proportionally_spaced_without_serifs:
			fontTf = Typeface.create("Arial", type);
			break;
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Dom:
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Default_Casual:
			fontTf = Typeface.create("Casual", type);
			break;
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Coronet:
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Default_Cursive:
			fontTf = Typeface.create("Coronet", type);
			break;
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Gothic:
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Default_Small_capitals:
			fontTf = Typeface.create("Gothic", type);
			break;
		//new added	
		case voSubTitleFormatSetting.VOOSMP_FontStyle_MonospaceSansSerif:
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Monospaced:
			fontTf = Typeface.create(Typeface.MONOSPACE, 0);
			break;
		case voSubTitleFormatSetting.VOOSMP_FontStyle_ProportionalSansSerif:
		case voSubTitleFormatSetting.VOOSMP_FontStyle_SansSerif:
			fontTf = Typeface.create(Typeface.SANS_SERIF, 0);
			break;
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Default_Proportionally_spaced_with_serifs:
		case voSubTitleFormatSetting.VOOSMP_FontStyle_ProportionalSerif:
		case voSubTitleFormatSetting.VOOSMP_FontStyle_Serif:
			fontTf = Typeface.create(Typeface.SERIF, 0);
			break;
		default:
			voLog.i(TAG, "Font Style is :%d, ui set to default.",id);
			break;
				
		}

		return fontTf;
	}
	
    /**
     * 
	 * preview Subtitle
	*/
	public void previewSubtitle(voOSPreviewSubtitleInfo info){
		this.clearWidget();
		this.setSurfaceView(info.getView());
		String s = info.getSampleText();
		voSubtitleInfo subtInfo = new voSubtitleInfo();
		subtInfo.maxDuration=-1;
		subtInfo.timeStamp = 0;
		voSubtitleInfoEntry subtEnt = new voSubtitleInfoEntry();
		subtEnt.getSubtitleRectInfo().rectDraw.left = 2;
		subtEnt.getSubtitleRectInfo().rectDraw.top = 2;
		subtEnt.getSubtitleRectInfo().rectDraw.right = 98;
		subtEnt.getSubtitleRectInfo().rectDraw.bottom = 98;
		subtInfo.subtitleEntry.add(subtEnt);
		//subtEnt.subtitleRectInfo.
		voSubtitleTextRowInfo row = new voSubtitleTextRowInfo();
		subtEnt.subtitleDispInfo.textRowInfo.add(row);
		row.textRowDes.horizontalJustification = 0;
		row.textRowDes.dataBox.rectDraw.left=10;
		row.textRowDes.dataBox.rectDraw.right=90;
		row.textRowDes.dataBox.rectDraw.top=30;
		row.textRowDes.dataBox.rectDraw.bottom=60;
		row.textRowDes.dataBox.rectFillColor = 0xff000000;
		//row.textRowDes.dataBox.rectDisplayEffct.
		voSubtitleTextInfoEntry textEnt = new voSubtitleTextInfoEntry();
		row.textInfoEntry.add(textEnt);
		textEnt.stringInfo.fontInfo.fontColor=0xffffffff;
		textEnt.stringInfo.fontInfo.fontSize = 0;
		textEnt.stringInfo.charEffect.EdgeColor=0;
		textEnt.stringInfo.charEffect.EdgeType=0;
		textEnt.stringInfo.charEffect.Italic=0;
		textEnt.stringInfo.charEffect.Underline=0;
		textEnt.stringText = s;
		
		this.mSubtitleInfoArray.add(subtInfo);
		this.mSubtitleInfo = subtInfo;
		
		mIsPreview = true;
		setXYRate(1.0f);
		createAndShowChildViews();
		this.show(true);
	}
    /**
     * 
	 * clear all widgets
	*/
	public void clearWidget() {
		if( llWindow1!=null)
		{
			textViewOfRows = null;
			llWindow1.removeAllViews();
			if(rlMain!=null)
				rlMain.removeView(llWindow1);
			llWindow1 = null;
			voLog.v(TAG," clearWidget rlMain.removeView(llWindow1)");  
		}
		mSubtitleInfoArray.clear();
		mSubtitleInfo = null;
		voLog.v(TAG," clearWidget ");  
	}
	 
    /**
     * 
	 * set Thumbnail PlayList url
	*/
	public void setThumbnailPlayListURL(String url) {
//		thumbnailPlayList = new voThumbnailPlayList();
//		final String url2 = url;
//		new Thread(new Runnable(){
//
//			@Override
//			public void run() {
//				thumbnailPlayList.setURL(url2);
//			}
//			
//		}).start();
	}
	
	/**
	 * get custom Settings interface
	 * 
	 * @return CCSettings interface to set some settings
	 */
	public voSubTitleFormatSettingImpl getSettings()
	{
		if(settings == null)
			settings = new voSubTitleFormatSettingImpl();
		return settings;
	}
	
	/**
	 * set layout to contain closed caption
	 * 
	 * @param rl the layout contains closed caption
	 */
	public void setMainLayout(ViewGroup rl)
	{
		if(rl == rlMain) return;
		voLog.v(TAG,"setMainLayout ,shoud call clearWidget()"); 
		clearWidget();
		
		//change parent of llWindow1;
//		if(this.llWindow1!=null){
//			ViewParent vw = llWindow1.getParent();
//			
//			if(vw!=null){
//				RelativeLayout rllt = new RelativeLayout(this.mainActivity);
//				Class cls = rllt.getClass();
//				if(cls.isInstance(vw))
//				{
//					RelativeLayout lt = (RelativeLayout)vw;
//					lt.removeView(llWindow1);
//					voLog.v(TAG,"setMainLayout lt.removeView(llWindow1) "); 
//				}
//			}
//			rl.addView(llWindow1);
//			voLog.v(TAG,"setMainLayout rl.addView(llWindow1) "); 
//		}
		rlMain = rl;
	}
	
	/**
	 * set surface view related to closed caption
	 * 
	 * @param vw the surface view related to closed caption
	 */
	public void setSurfaceView(View vw)
	{
		voLog.v(TAG,"enter setSurfaceView "); 
		clearWidget();
		vwSurface = vw;
		if(vwSurface!=null){
			Object bj = vwSurface.getParent();
			if(bj!=null){
				if(bj instanceof ViewGroup){
					ViewGroup rl = (ViewGroup)(vwSurface.getParent());
					if(rl!=null){
						voLog.v(TAG,"enter setSurfaceView setMainLayout "); 
				        setMainLayout( rl);
					}
				}
				else
					setMainLayout( null);
				
			}
			if(mainActivity==null)
				mainActivity = vwSurface.getContext();
		}
	}
	
	/**
	 * set current Activity
	 * 
	 * @param ac the current Activity object
	 */
	public void setActivity(Context  ac)
	{
		voLog.v(TAG,"setActivity "); 
		mainActivity = ac;
	}
	
	/**
	 * set Aspect ratio of display region for closed caption
	 * 
	 * @param rate the Aspect ratio, for example, width is 600, height is 480, then the Aspect ratio is 600/480.0
	 */
	public void setXYRate(float rate)
	{
		this.xyRate = rate;
		if( llWindow1 == null ) 
			return;
		
		RelativeLayout.LayoutParams rlp = new RelativeLayout.LayoutParams(LayoutParams.WRAP_CONTENT,LayoutParams.FILL_PARENT);
		width = rlMain.getWidth();//getRight() - rlMain.getLeft();
		height = rlMain.getHeight();
		
		
		if((width<=0 || height<=0) && mainActivity!=null){
			DisplayMetrics dm  = new DisplayMetrics();
			WindowManager man = (WindowManager)mainActivity.getSystemService(Context.WINDOW_SERVICE);
			man.getDefaultDisplay().getMetrics(dm);
			if(!mIsPreview){
				width = dm.widthPixels;//
				height = dm.heightPixels;//
				if(dm.density>0)
					density = dm.density;
			}
		}
		if(!mIsPreview){
			int widthmax = width;
			int heightmax = height;
	
			if(width>(int)(height*xyRate))
			{
				width = (int)(height*xyRate);
				rlp.leftMargin = (widthmax-width)/2;
				rlp.width = width;
				rlp.height = height;
				rlp.topMargin = 0;
			}
			else
			{
				height = (int)(width/xyRate);
				rlp.topMargin = (heightmax-height)/2;
				rlp.width = width;
				rlp.height = height;
				rlp.leftMargin = 0;
			}
		}
		if(!mIsPreview)
			llWindow1.setLayoutParams(rlp);
	}
	
	/**
	 * convert raw data to voSubtitleInfo object
	 * 
	 * @param parc the raw data from sdk engine
	 * @return voSubtitleInfo object
	 */
	public voSubtitleInfo parcelToSubtitleInfo(Parcel parc)
	{
		if(mIsPreview)
			return null;
		if(parc == null)
			return null;
		parc.setDataPosition(0);
        if (parc.dataAvail() == 0) {
            return null;
        }
        
        voSubtitleInfo subtitleInfo = new voSubtitleInfo();
        subtitleInfo.parse(parc);
        return subtitleInfo;
		
	}
	
	/**
	 * set raw data into closed caption manager
	 * 
	 * @param parc the raw data from sdk engine
	 * @param clearOldData whether to clear old data of closed caption
	 * @return true when success, false when fail
	 */
	public boolean setData(Parcel parc, boolean clearOldData) {
		if(parc == null)
	        return false;
		if(mParcel!=null)
			mParcel.recycle();
		parc.setDataPosition(0);
		mParcel = parc;
		mSubtitleInfoArray.clear();
		if(!clearOldData)
			metadata_Arrive = true;

        if (!parseParcel()) {
        	//
        	if(clearOldData) this.mSubtitleInfo = null;
        	this.clearWidget();
        	//if(this.llWindow1!=null)
        	//	llWindow1.setVisibility(View.GONE);
        	return false;
        }
        if(clearOldData) this.mSubtitleInfo = null;
        if(metadata_Arrive && mSubtitleInfoArray.size()==1){
        	voSubtitleInfo tmp = mSubtitleInfo;
        	mSubtitleInfo = mSubtitleInfoArray.get(0);
        	createImageView(true);
        	mSubtitleInfo = tmp;
        }
        return true;
    }
    private String reverseString(CharSequence csText)
    {
		int len = csText.length();
		String strReverse = "";
		for(int i_char = len -1; i_char >= 0; i_char --){
			strReverse += csText.charAt(i_char);
		}
		return strReverse;
    }
    private boolean parseParcel() {
        mParcel.setDataPosition(0);
        if (mParcel.dataAvail() == 0) {
            return false;
        }
        
        voSubtitleInfo subtitleInfo = new voSubtitleInfo();
        subtitleInfo.parse(mParcel);
        mSubtitleInfoArray.add(subtitleInfo);
        //voLog.v("Subtitle","timeStamp Subtitle = %d ",subtitleInfo.timeStamp);         //this.mSubtitleInfo = subtitleInfo;
        return true;

    }
    
    public boolean isEmptySubtitleInfo() {
        voSubtitleInfo subtitleInfo = this.mSubtitleInfo;
        if(subtitleInfo == null)
        {
        	if(mSubtitleInfoArray.size()>0)
        		subtitleInfo = mSubtitleInfoArray.get(mSubtitleInfoArray.size()-1);
        }
        if(subtitleInfo != null){
        	if(subtitleInfo.subtitleEntry.size()>=1)
        	{
        		if(subtitleInfo.subtitleEntry.get(0).subtitleDispInfo.getTextRowInfo().size() >= 1)
        		{
	        		voSubtitleTextRowInfo row = subtitleInfo.subtitleEntry.get(0).subtitleDispInfo.getTextRowInfo().get(0);
	        		if(row.textInfoEntry.size()>=1)
	        		{
	        			if(row.textInfoEntry.get(0).stringText.length()==0)
	        			{
	        				return true;
	        			}
	        		}
        		}
        	}
        }
        //voLog.v("Subtitle","timeStamp Subtitle = %d ",subtitleInfo.timeStamp);         //this.mSubtitleInfo = subtitleInfo;
        return false;

    }
    
	/**
	 * show closed caption or not
	 * 
	 * @param bShow true is to show, false is to hide
	 */
    public void show(boolean bShow)
	{
		mShow = bShow;
		if(!bShow)
		{
			if(llWindow1!=null)
			{
				if(llWindow1.getVisibility()==View.VISIBLE)
					llWindow1.setVisibility(View.GONE);
				//llWindow1.removeAllViews();
				//this.removeImageViews();
			}
			if(textViewOfRows!=null){
				textViewOfRows.enableDraw(false);
			}
//			if(mSubtitleInfoArray!=null)
//				this.mSubtitleInfoArray.clear();
//			if(mSubtitleInfo==null || mSubtitleInfo.subtitleEntry == null) return;
//			
//			for(int i = 0; i<mSubtitleInfo.subtitleEntry.size(); i++)
//			{
//				voSubtitleInfoEntry info = mSubtitleInfo.subtitleEntry.get(i);
//				info.mChildViewArr.clear();
//			}
//			
//			mSubtitleInfo = null;
	        voLog.v(TAG,"show ");         //this.mSubtitleInfo = subtitleInfo;
		}
		else
		{
			if(llWindow1!=null)
			{
				if(llWindow1.getVisibility()!=View.VISIBLE)
					llWindow1.setVisibility(View.VISIBLE);
			}
			if(textViewOfRows!=null){
				textViewOfRows.enableDraw(true);
			}
	        voLog.v(TAG,"hide ");         //this.mSubtitleInfo = subtitleInfo;
			
		}
	}
	
	/**
	 * check the closed caption which should show at current time
	 * 
	 * @param curTime the current time
	 */
    public void checkViewShowStatus(int curTime)
	{
		if(!mShow || mIsPreview)
			return;
		if(rlMain == null) return;
		if(vwSurface!=null){
			RelativeLayout rl = (RelativeLayout)(vwSurface.getParent());
			if(rl!=this.rlMain){
		        setMainLayout( rl);
			}
		}
		
		if(checkxyRate()){
			mSubtitleInfo = null;
			setXYRate(xyRate);
		}

		if(mSubtitleInfo!=null)
		{
			boolean bFind = false;
			if((mSubtitleInfo.maxDuration == -1 || mSubtitleInfo.timeStamp+mSubtitleInfo.maxDuration>curTime) && (mSubtitleInfo.timeStamp<curTime))
				bFind = true;
			
	//		voLog.i(TAG, "bFind is %s, maxDuration is %d, timeStamp is %d", Boolean.toString(bFind), mSubtitleInfo.maxDuration, mSubtitleInfo.timeStamp);
			
			if(!bFind && mSubtitleInfo.timeStamp>=0)
			{
				mSubtitleInfo = null;
			}
			
			if(bFind && !metadata_Arrive)
			{
				if(mSubtitleInfo.maxDuration == -1 && curTime - mSubtitleInfo.timeStamp>15000)
				{
					mSubtitleInfoArray.clear();
					mSubtitleInfo = null;
					clearWidget();
				}
				
			}
		}
		
		if(mSubtitleInfo == null && mSubtitleInfoArray.size()>0)
		{
	//		voLog.i(TAG, "subtitle show 2");
			for(int k = 0; k<mSubtitleInfoArray.size(); k++)
			{
	//			voLog.i(TAG, "subtitle show 3");
				voSubtitleInfo subtInfo = mSubtitleInfoArray.get(k);
				if(subtInfo.timeStamp>curTime)
					continue;
				for(int i = 0; subtInfo.subtitleEntry!=null && i<subtInfo.subtitleEntry.size() ; i++)
				{
					voSubtitleInfoEntry info = subtInfo.subtitleEntry.get(i);
					if(info.duration==0xffffffff || info.duration + subtInfo.timeStamp > curTime)
					{
						//ok
						if(!subtInfo.equals(mSubtitleInfo))
						{
							mSubtitleInfo = subtInfo;
							//Check if clear
							if ((info.subtitleDispInfo.textRowInfo == null &&
									info.subtitleDispInfo.imageInfo == null) ||
									(info.subtitleDispInfo.textRowInfo.size() == 0 &&
									info.subtitleDispInfo.imageInfo.size() == 0))
							{
								voLog.i(TAG, "ClosedCaption overtime null point. subtitleDispInfo.textRowInfo" + info.subtitleDispInfo.textRowInfo);
								//if(!metadata_Arrive)
								//clearWidget();
								if(textViewOfRows!=null)
									this.textViewOfRows.invalidate();
								return;
							}
							//to create child view
							createAndShowChildViews();
							return;
						}
					}else
					{
						if (llWindow1 != null)
						{
							voLog.i(TAG, "ClosedCaption overtime, Duration is %d, subtitle timestamp is %d, current timestamp is %d ",
									info.duration, subtInfo.timeStamp, curTime);
							clearWidget();
							return;
						}
					}
				}
			}
		}
		if( mSubtitleInfo == null || mSubtitleInfo.subtitleEntry == null)
		{
			return;
		}
		
	}

    private boolean checkxyRate(){
    	if(rlMain!=null ){
			int width1 = rlMain.getWidth();
			int height1 = rlMain.getHeight();
			if((width1<=0 || height1<=0)&& mainActivity!= null){
				DisplayMetrics dm  = new DisplayMetrics();
				WindowManager man = (WindowManager)mainActivity.getSystemService(Context.WINDOW_SERVICE);
				man.getDefaultDisplay().getMetrics(dm);
				//mainActivity.getWindowManager().getDefaultDisplay().getMetrics(dm);
				width1 = dm.widthPixels;//
				height1 = dm.heightPixels;//
			}
			//voLog.v("ClosedCaption", "width1=%d,width1=%d;height1=%d,height=%d", width1,width,height1,height);
			if((widthOld != width1 || heightOld != height1))
			{
				width = width1;
				height = height1;
				heightOld = height1;
				widthOld = width1;
				return true;
			}

    	}
    	return false;
    }
//    private void recreateTextView(){
//		if(textViewOfRows== null)
//			return;
//		llWindow1.removeAllViews();
//		RelativeLayout.LayoutParams rlp = new RelativeLayout.LayoutParams(LayoutParams.FILL_PARENT,LayoutParams.FILL_PARENT);
//		//textViewOfRows = new TextOutLinesView(mainActivity);
//		rlp = new RelativeLayout.LayoutParams(LayoutParams.FILL_PARENT,LayoutParams.FILL_PARENT);
//		llWindow1.addView(textViewOfRows,rlp);
//		llWindow1.setLayoutParams(rlp);
//    	
//    }
    private void createImageView(boolean removeOldImage){
    	if(llWindow1 == null)
    		this.createAndShowChildViews();
    	if(llWindow1 == null ||mSubtitleInfo == null)
    		return;
    	
    	//if(removeOldImage){
    	//	removeImageViews();
    	//}
    	if(!metadata_Arrive)
    		return;
		for(int i = 0; i<mSubtitleInfo.subtitleEntry.size(); i++)
		{
			voSubtitleInfoEntry info = mSubtitleInfo.subtitleEntry.get(i);
			voSubtitleDisplayInfo dispInfo = info.subtitleDispInfo;
			if(dispInfo.imageInfo != null)
			{
				if( dispInfo.imageInfo.size()>0)
					removeImageViews();
				boolean imageCreated = false;
				for(int j = 0; j < dispInfo.imageInfo.size() ; j++)
				{
					voSubtitleImageInfo imgInfo = dispInfo.imageInfo.get(j);
					if( imgInfo.imageData == null)
						continue;
					if( imgInfo.imageData.getPicData()==null)
						continue;
					//if( imgInfo.imageInfoDescriptor == null)
					//	continue;
					
					LinearLayoutShowBorder llRow = new LinearLayoutShowBorder(mainActivity);
					llRow.setTag(llWindow1);
					imageCreated = true;

					int nLeft = -1;
					int nRight = -1;
					int nTop = -1;
					int nBottom = -1;
					if( imgInfo.imageInfoDescriptor != null){
						nLeft = toRealX(imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.left);
						nRight = toRealX(imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.right);
						nTop = toRealY(imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.top);
						nBottom = toRealY(imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.bottom);
						voLog.i(TAG, "Rect left is %d, top is %d, right is %d, bottom is %d. \nReal rect left is %d, top is %d, right is %d, bottom is %d.",
								imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.left, imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.top, 
								imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.right, imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.bottom,
								nLeft, nTop, nRight, nBottom);
					}

					if(!(nLeft<=0 && nRight<=0))
					{
					//	RelativeLayout.LayoutParams rlp = new RelativeLayout.LayoutParams(LayoutParams.WRAP_CONTENT,LayoutParams.WRAP_CONTENT);
						RelativeLayout.LayoutParams rlp = new RelativeLayout.LayoutParams(nRight - nLeft, nBottom - nTop);
						rlp.leftMargin = nLeft;
						rlp.topMargin = nTop;
						llWindow1.addView(llRow,rlp);
						llRow.setVerticalGravity(Gravity.CENTER_VERTICAL);
						llRow.setHorizontalGravity(Gravity.CENTER_HORIZONTAL);
					}
					else
					{
						RelativeLayout.LayoutParams rlp = new RelativeLayout.LayoutParams(LayoutParams.FILL_PARENT,LayoutParams.FILL_PARENT);
						llRow.setVerticalGravity(Gravity.CENTER_VERTICAL);
						llRow.setHorizontalGravity(Gravity.CENTER_HORIZONTAL);
						llWindow1.addView(llRow,rlp);
					}

					if( imgInfo.imageInfoDescriptor != null){
					llRow.setBackgroundColor(imgInfo.imageInfoDescriptor.imageRectInfo.rectFillColor);
						if(imgInfo.imageInfoDescriptor.imageRectInfo.rectBorderType>0)
						{
							llRow.setBorder(2, imgInfo.imageInfoDescriptor.imageRectInfo.rectBorderColor);
						}
					}
					
					Bitmap bm = null;
					{
					    if(metadata_Arrive){
					        android.graphics.Matrix mt = new android.graphics.Matrix();
					        Options options = new Options();
	                         //options.inScaled = true;
	                         int h = rlMain.getHeight();
	                          int w = rlMain.getWidth();
					        Bitmap resizeBmp = BitmapFactory.decodeByteArray(imgInfo.imageData.picData, 0, imgInfo.imageData.picData.length,options);//decodeStream(bis);
					        float scale = 0;
					        if(h*resizeBmp.getWidth() >w*resizeBmp.getHeight())
                            {
                                scale = (float)w/(float)resizeBmp.getWidth();
                                
                            }else{
                                scale = (float)h/(float)resizeBmp.getHeight();
                               
                            }
					        mt.postScale(scale, scale);
                            bm = Bitmap.createBitmap(resizeBmp, 0, 0, resizeBmp.getWidth(), resizeBmp.getHeight(), mt, true);

					        voLog.e(TAG, "subtitle decodeByteArray %d  %d   ",llWindow1.getWidth(),llWindow1.getHeight());
					        if(llWindow1!=null && height!=0){
					            setXYRate(w/(float)h);
					        }
					        voLog.e(TAG, "subtitle decodeByteArray %d  %d   %d  %d ",llWindow1.getWidth(),llWindow1.getHeight(),w,h);
					        voLog.e(TAG, "subtitle decodeByteArray %d  %d   %d  %d",resizeBmp.getWidth(),resizeBmp.getHeight(),bm.getWidth(),bm.getHeight());
					    }else{
					        bm = BitmapFactory.decodeByteArray(imgInfo.imageData.picData, 0, imgInfo.imageData.picData.length);//decodeStream(bis);
					    }
					}
						
   					
					if(bm!=null){
   						ImageView iv = new ImageView(mainActivity);
	                     
	                    //iv.setBackgroundColor(0x00000000);
   						iv.setImageBitmap(bm); 
	   					llRow.addView(iv);
	   					if(!metadata_Arrive)
	   					{
	   						
		   					llRow.setBackgroundColor(0x00000000);
		   					llRow.setGravity( Gravity.CENTER);
		   					
		   					if(!(nLeft<=0 && nRight<=0))
		   					{
			   					LayoutParams para;  
			   			        para = iv.getLayoutParams();  
			   			        para.width = nRight - nLeft;
			   			        para.height = nBottom - nTop;
			   			        iv.setLayoutParams(para);
		   					}
	   						
	   					}else{
		   					llRow.setBackgroundColor(0xFF000000);
		   					llRow.setGravity( Gravity.CENTER);
		   					//iv.setImageBitmap(bm);
		   					if(!(nLeft<=0 && nRight<=0))
		   					{
			   					LayoutParams para;  
			   			        para = iv.getLayoutParams();
			   			        para.width = nRight - nLeft;
			   			        para.height = nBottom - nTop;
			   			        iv.setLayoutParams(para);
		   					}
	   					}
					}
   					//info.mChildViewArr.add(llRow);
				}
				if(imageCreated){
					if(textViewOfRows!=null){
						textViewOfRows.bringToFront();
					}

				}
			}
			}
    }
	private void createAndShowChildViews()
	{
		if(mainActivity == null || mSubtitleInfo == null) return;
		if(rlMain == null) return;
		//show(true);
		voLog.v(TAG," enter createAndShowChildViews ");  
		if(llWindow1==null )
		{
			llWindow1 = new RelativeLayout(mainActivity);
			RelativeLayout.LayoutParams rlp = new RelativeLayout.LayoutParams(LayoutParams.FILL_PARENT,LayoutParams.FILL_PARENT);
			rlMain.addView(llWindow1,rlp);
			
			textViewOfRows = new TextOutLinesView(mainActivity);
			rlp = new RelativeLayout.LayoutParams(LayoutParams.FILL_PARENT,LayoutParams.FILL_PARENT);
			llWindow1.addView(textViewOfRows,rlp);
			rlp = null;
			
			setXYRate(xyRate);
			
			voLog.v(TAG," createAndShowChildViews llWindow1.addView(textViewOfRows,rlp)");  

		}
		else{
			if(checkxyRate()){
					setXYRate(xyRate);
					//recreateTextView();
			}
		}
//		if(llWindow1.getVisibility()!=View.VISIBLE)
//			llWindow1.setVisibility(View.VISIBLE);
		//if(!metadata_Arrive)
		//	removeImageViews();
		
		if(mSubtitleInfo.subtitleEntry == null) return;
		
		for(int i = 0; i<mSubtitleInfo.subtitleEntry.size(); i++)
		{
			voSubtitleInfoEntry info = mSubtitleInfo.subtitleEntry.get(i);
			voSubtitleDisplayInfo dispInfo = info.subtitleDispInfo;
			if(dispInfo.textRowInfo != null)
			{
				for(int j = 0; j < dispInfo.textRowInfo.size() ; j++)
				{
					voSubtitleTextRowInfo rowInfo = dispInfo.textRowInfo.get(j);
					startAnimation( rowInfo, llWindow1);
				
				}
			}
		}

		
		textViewOfRows.invalidate();

		createImageView(false);
		
	}
	private void removeImageViews()
	{
		if(llWindow1 == null) return;
		if(textViewOfRows != null)
		{
			textViewOfRows.invalidate();
		}
		
		while(true)
		{
			View vw = llWindow1.findViewWithTag(llWindow1);
			if(vw == null)
				break;
			llWindow1.removeView(vw);
			
		}
	}
	private void startAnimation(voSubtitleTextRowInfo rowInfo, RelativeLayout llRow)
	{
		//start animate
		if(rowInfo.textRowDes.dataBox.rectDisplayEffct.effectType>0)
		{
			if(rowInfo.textRowDes.dataBox.rectDisplayEffct.effectType == 1)//fade
			{
				AlphaAnimation myAnimation_Alpha=new AlphaAnimation(0.1f, 1.0f);
				myAnimation_Alpha.setDuration(rowInfo.textRowDes.dataBox.rectDisplayEffct.effectSpeed*1000);
				llRow.startAnimation(myAnimation_Alpha);
			}
			if(rowInfo.textRowDes.dataBox.rectDisplayEffct.effectType == 2)//wipe
			{
				//TranslateAnimation(float fromXDelta, float toXDelta, float fromYDelta, float toYDelta)
				float fromXDelta,  toXDelta,  fromYDelta,  toYDelta;
				fromXDelta = toXDelta = 0;//(float)nLeft;
				fromYDelta = toYDelta = 0;//(float)nTop;
				if(rowInfo.textRowDes.dataBox.rectDisplayEffct.effectDirection == 0)//left to right
				{
					fromXDelta = -width;
				}
				if(rowInfo.textRowDes.dataBox.rectDisplayEffct.effectDirection == 1)//right to left
				{
					fromXDelta = width;
				}
				if(rowInfo.textRowDes.dataBox.rectDisplayEffct.effectDirection == 2)//top to bottom
				{
					fromYDelta = -height;
				}
				if(rowInfo.textRowDes.dataBox.rectDisplayEffct.effectDirection == 3)//b to t
				{
					fromYDelta = height;
				}
				TranslateAnimation myAnimation_wipe=new TranslateAnimation(fromXDelta,  toXDelta,  fromYDelta,  toYDelta);
				myAnimation_wipe.setDuration(rowInfo.textRowDes.dataBox.rectDisplayEffct.effectSpeed*1000);
				llRow.startAnimation(myAnimation_wipe);
			}
			
		}
		
	}
    private int toRealX(int x)
    {
    	return width*x/100;
    }
    private int toRealY(int y)
    {
    	return height*y/100;
    }
    private float getFontSize(voSubtitleTextInfoEntry textInfo)
    {
		float rate2 = 0.75f;
		//textInfo.stringInfo.fontInfo.fontSize = 200;
		int nFont = textInfo.stringInfo.fontInfo.fontSize;
		if(nFont == 1)
		{
			rate2 = .7f;
		}
		if(nFont == 2)
		{
			rate2 = .8f;
		}
		if(nFont >1000 && nFont<10000)
		{
			rate2 = (.75f * (nFont-1000))/100.f ;
		}
		if(settings != null && settings.fontSizeEnable){
			rate2*=settings.fontSizeMedium;
		}
		
		//voLog.i(TAG, "getFontSize is %d, float=%.3f",textInfo.stringInfo.fontInfo.fontSize, rate2);
    	return rate2;
    }
	
	/**
	 * font information interface
	 * 
	 */
	public class voSubtitleFontInfo
	{
		/**
		 * get font size
		 * 
		 * @return 0:STANDARD, 1:SMALL, 2:LARGE, 102:BOLD, 103:STANDARD but no bold
		 */
		public int getFontSize() {
			return fontSize;
		}
		
		/**
		 * get font style
		 * 
		 * @return 0:FontStyle_Default,1:FontStyle_Monospaced_with_serifs, 2:FontStyle_Default_Proportionally_spaced_with_serifs,
		 * 3:FontStyle_Default_Monospaced_without_serifs, 4:FontStyle_Default_Proportionally_spaced_without_serifs, 
		 * 5:FontStyle_Default_Casual, 6:FontStyle_Default_Cursive, 7:FontStyle_Default_Small_capitals
		 * 8:FontStyle_Monospaced, 9:FontStyle_SansSerif, 10:FontStyle_Serif,11:FontStyle_ProportionalSansSerif,
		 * 12:FontStyle_ProportionalSerif,
		 * 13:FontStyle_Times_New_Roman,14:FontStyle_Courier,15:FontStyle_Helvetica,16:FontStyle_Arial,
		 * 17:FontStyle_Dom,18:FontStyle_Coronet,19:FontStyle_Gothic,20:FontStyle_MonospaceSansSerif

		 */
		public int getFontStyle() {
			return fontStyle;
		}
		
		/**
		 * get font color
		 * 
		 * @return font color
		 */
		public int getFontColor() {
			return fontColor;
		}
		private int	fontSize=0;///<0:STANDARD,1:SMALL, 2:LARGE, 102:BOLD, 103:STANDARD but no bold
		private int	fontStyle=0;///<refer to voSubtitleFontStyle
		private int	fontColor=0;
		private voSubtitleFontInfo(){}
		private boolean parse(Parcel parc)
		{
	        int type = parc.readInt();
        	if (type != KEY_FONT_INFO)
        	{//The size is 0
        		parc.setDataPosition(parc.dataPosition() - 4);
        		return false;
        	}
        	type = parc.readInt();
        	if (type != KEY_FONT_INFO_SIZE)
        	{
        		return false;
        	}
        	fontSize = parc.readInt();
        	//fontSize = 10024;
        	
        	type = parc.readInt();
        	fontStyle = parc.readInt();
        	type = parc.readInt();
        	fontColor = parc.readInt();
        	if(settings != null)  
        	{
        		fontColor = settings.converColor(fontColor,settings.fontColor,settings.fontColorEnable );
        		fontColor = settings.converColorOpacityRate(fontColor, settings.fontColorOpacityRate, settings.fontColorOpacityRateEnable);
        	}
			return true;
		}
	};
	/**
	 * font effect interface
	 * 
	 */
	public class voSubtitleFontEffect
	{
		/**
		 * get text tag
		 * 
		 * @return tag value
		 */
		public int getTextTag() {
			return TextTag;
		}
		
		/**
		 * get italic 
		 * 
		 * @return 0:normal, 1:Italic
		 */
		public int getItalic() {
			return Italic;
		}
		
		/**
		 * get underline
		 * 
		 * @return 0:normal, 1:Underline
		 */
		public int getUnderline() {
			return Underline;
		}
		
		/**
		 * 
		 * get edge type
		 * 
		 * @return 0:NONE, 1:RAISED, 2:DEPRESSED, 3:UNIFORM, 4:LEFT_DROP_SHADOW, 5:RIGHT_DROP_SHADOW
		 */
		public int getEdgeType() {
			return EdgeType;
		}
		
		/**
		 * 
		 * get offset
		 * 
		 * @return 0: NORMAL, 1:SUBSCRIPT, 2:SUPERSCRIPT
		 */
		public int getOffset() {
			return Offset;
		}
		
		/**
		 * get edge color
		 * 
		 * @return font edge color
		 */
		public int getEdgeColor() {
			return EdgeColor;
		}
		
		private int	TextTag=0;///<ref to voSubtitleFondTextTag
		private int	Italic=0;
		private int	Underline=0;
		private int	EdgeType=0;///<0:NONE,1:RAISED,2:DEPRESSED,3:UNIFORM,4:LEFT_DROP_SHADOW,5:RIGHT_DROP_SHADOW
		private int	Offset=0;///<0: NORMAL,1:SUBSCRIPT,2:SUPERSCRIPT
		private int EdgeColor=0;
		private voSubtitleFontEffect(){}
		
		private boolean parse(Parcel parc)
		{
	        int type = parc.readInt();
        	if (type != KEY_FONT_EFFECT)
        	{
        		parc.setDataPosition(parc.dataPosition() - 4);
        		return false;
        	}
        	type = parc.readInt();
        	TextTag = parc.readInt();
        	type = parc.readInt();
        	Italic = parc.readInt();
        	type = parc.readInt();
        	Underline = parc.readInt();
        	type = parc.readInt();
        	EdgeType = parc.readInt();
        	type = parc.readInt();
        	Offset = parc.readInt();
        	type = parc.readInt();
        	EdgeColor = parc.readInt();
        	if(settings != null)  
        	{
        		if(settings.fontUnderlineEnable)
        		{
        			Underline = settings.fontUnderline;
        		}
        		if(settings.fontItalicEnable)
        		{
        			Italic = settings.fontItalic;
        		}
        		if(settings.edgeTypeEnable)
        		{
        			EdgeType = settings.edgeType;
        		}
        		if(settings.edgeColorEnable)
        		{
        			EdgeColor = settings.edgeColor;
        		}
        		EdgeColor = settings.converColor(EdgeColor,settings.edgeColor,settings.edgeColorEnable );
        		EdgeColor = settings.converColorOpacityRate(EdgeColor, settings.edgeColorOpacityRate, settings.edgeColorOpacityRateEnable);
        	}
       	
			return true;
		}
	};

	/**
	 * display effect interface
	 * 
	 */
	public class voSubtitleDisPlayEffect
	{
		/**
		 * 
		 * get effect type
		 * 
		 * @return 0:SNAP, 1:FADE, 2:WIPE
		 */
		public int getEffectType() {
			return effectType;
		}
		
		/**
		 * get effect direction
		 * 
		 * @return 0:LEFT_TO_RIGHT; 1:RIGHT_TO_LEFT; 2:TOP_TO_BOTTOM; 3:BOTTOM_TO_TOP
		 */
		public int getEffectDirection() {
			return effectDirection;
		}
		
		/**
		 * get effect speed
		 * 
		 * @return unit is second
		 */
		public int getEffectSpeed() {
			return effectSpeed;
		}
		
		private int					effectType = 0;///<0:SNAP, 1:FADE, 2:WIPE
		private int					effectDirection=0;///<0:LEFT_TO_RIGHT;1:RIGHT_TO_LEFT;2:TOP_TO_BOTTOM;3:BOTTOM_TO_TOP
		private int					effectSpeed=0;///<second
		private voSubtitleDisPlayEffect(){}
		
		private boolean parse(Parcel parc)
		{
	        int type = parc.readInt();
        	if (type != KEY_DISPLAY_EFFECTTYPE)
        	{
        		parc.setDataPosition(parc.dataPosition() - 4);
        		return false;
        	}
        	effectType = parc.readInt();
        	type = parc.readInt();
        	effectDirection =  parc.readInt();
        	type = parc.readInt();
        	effectSpeed =  parc.readInt();
			return true;
		}
	};

	/**
	 * rectangle information interface
	 * 
	 */
	private class voRect
	{
		/**
		 * get rectangle for drawing
		 * 
		 * @return drawing rectangle region
		 */
		public Rect getRectDraw() {
			return rectDraw;
		}
		
		/**
		 * get rectangle border type
		 * 
		 * @return 0:NONE, 1:RAISED, 2:DEPRESSED, 3:UNIFORM,  4:SHADOW_LEFT, 5:SHADOW_RIGH
		 */
		public int getRectBorderType() {
			return rectBorderType;
		}
		
		/**
		 * get rectangle border color
		 * 
		 * @return border color
		 */
		public int getRectBorderColor() {
			return rectBorderColor;
		}
		
		/**
		 * get color to fill the rectangle 
		 * 
		 * @return color to fill
		 */
		public int getRectFillColor() {
			return rectFillColor;
		}
		/**
		 * set rectFillColor
		 * 
		 * @param rectFillColor, the color to fill
		 * @return void
		 */
		private void setRectFillColor(int rectFillColor) {
			this.rectFillColor = rectFillColor;
		}
		
		/**
		 * get rectangle ZOrder
		 * 
		 * @return ZOrder value, More negative layer values are towards the viewer:for example :0,1,2
		 */
		public int getRectZOrder() {
			return rectZOrder;
		}
		
		/**
		 * get display effect of the rectangle
		 * 
		 * @return voSubtitleDisPlayEffect object
		 */
		public voSubtitleDisPlayEffect getRectDisplayEffect() {
			return rectDisplayEffct;
		}
		private Rect				rectDraw = new Rect();///<nBottom ==0&&nRight==0 indicate the width and height is unknow,so use the reasonable size for Rectangle
		private int					rectBorderType = 0;///<0:NONE, 1:RAISED, 2:DEPRESSED, 3:UNIFORM,  4:SHADOW_LEFT, 5:SHADOW_RIGHT
		private int					rectBorderColor = 0;
		private int					rectFillColor = 0; 
		private int					rectZOrder = 0;///< More negative layer values are towards the viewer:for example :0,1,2
		voSubtitleDisPlayEffect	rectDisplayEffct = new voSubtitleDisPlayEffect();
		private voRect(){}
		private boolean parse(Parcel parc)
		{
			int type = parc.readInt();
	        if (type != KEY_RECT_INFO) 
        	{
        		parc.setDataPosition(parc.dataPosition() - 4);
        		return false;
        	}
	        type = parc.readInt();
        	if (type != KEY_STRUCT_RECT)
        		return false;
        	//rectDraw = new Rect();
        	rectDraw.top = parc.readInt();
        	rectDraw.left = parc.readInt();
        	rectDraw.bottom = parc.readInt();
        	rectDraw.right = parc.readInt();
	        //if (type != KEY_RECT_INFO) 
        	//	return false;
        	type = parc.readInt();
        	rectBorderType =  parc.readInt();
        	type = parc.readInt();
        	rectBorderColor =  parc.readInt();
        	type = parc.readInt();
        	rectFillColor =  parc.readInt();
        	type = parc.readInt();
        	rectZOrder =  parc.readInt();
        	
        	//rectDisplayEffct = new voSubtitleDisPlayEffect();
        	rectDisplayEffct.parse(parc);
//        	rectDraw.top = 80;
//        	rectDraw.left = 10;
        	if(rectDraw.top>95) rectDraw.top = 95;
        	if(rectDraw.left>95) rectDraw.left = 95;
        	if(rectDraw.bottom>99)rectDraw.bottom = 99;
        	if(rectDraw.right>99) rectDraw.right = 99;
        	//if(rectDraw.top<=-1) rectDraw.top = 5;
        	//if(rectDraw.left<=-1) rectDraw.left = 5;
        	//if(rectDraw.bottom<=-1)
        	//	rectDraw.bottom = 99;
        	//if(rectDraw.right<=-1) 
        	//	rectDraw.right = 99;
            //voLog.v("CloseCaption","rectDraw CloseCaption l= %d;t=%d ",rectDraw.left,rectDraw.top);         //this.mSubtitleInfo = subtitleInfo;
			return true;
		}
	};

	/**
	 * string information class.
	 * 
	 */
	public class voSubtitleStringInfo
	{
		/**
		 * get font information
		 * 
		 * @return voSubtitleFontInfo object
		 */
		public voSubtitleFontInfo getFontInfo() {
			return fontInfo;
		}
		
		/**
		 * get char effect information
		 * 
		 * @return voSubtitleFontEffect object
		 */
		public voSubtitleFontEffect getCharEffect() {
			return charEffect;
		}
		
		private voSubtitleFontInfo		fontInfo =  new voSubtitleFontInfo();
		private voSubtitleFontEffect	charEffect = new voSubtitleFontEffect();
		private voSubtitleStringInfo(){}
		private boolean parse(Parcel parc)
		{
	        int type = parc.readInt();
        	if (type != KEY_STRING_INFO)
        	{//The size is 0
        		parc.setDataPosition(parc.dataPosition() - 4);
        		return false;
        	}
        	
        	fontInfo = new voSubtitleFontInfo();
        	if(!fontInfo.parse(parc))
        		return false;
        	charEffect = new voSubtitleFontEffect();
        	if(!charEffect.parse(parc))
        		return false;
       	
			return true;
		}
	};

	/**
	 * text information entry class.
	 * 
	 * 
	 * 
	 */
	public class voSubtitleTextInfoEntry
	{
		/**
		 * get text
		 * 
		 * @return string of an entry
		 */
		public String getStringText() {
			return stringText;
		}
		
		/**
		 * get string information
		 * 
		 * @return voSubtitleStringInfo object
		 */
		public voSubtitleStringInfo getStringInfo() {
			return stringInfo;
		}
		private String 							stringText = "";
		private voSubtitleStringInfo			stringInfo = new voSubtitleStringInfo();
		private voSubtitleTextInfoEntry()
		{
			
		}
		private boolean parse(Parcel parc)
		{
	        int type = parc.readInt();
        	if (type != KEY_TEXT_SIZE)
        	{//The size is 0
        		parc.setDataPosition(parc.dataPosition() - 4);
        		return false;
        	}
        	int size = parc.readInt();
        	type = parc.readInt();
        	if(size>0)
        	{
	        	byte[] dt = new byte[size];
	        	parc.readByteArray(dt);
	        	for(int j = 0; j<size; j+=2)
	        	{
	        		byte b1 = dt[j];
	        		dt[j] = dt[j+1];
	        		dt[j+1] = b1;
	        	}
	        	try {
					stringText = new String(dt,0,size,"utf-16");
					if(stringText.length()>0 && stringText.charAt(0) == '\n')
						stringText = stringText.substring(1);
			        //voLog.v("CloseCaption"," CloseCaption = %s", stringText); 
				} catch (UnsupportedEncodingException e) {
					e.printStackTrace();
				}
        	}
        	
        	stringInfo = new voSubtitleStringInfo();
        	if(!stringInfo.parse(parc))
        		return false;
       	
			return true;
		}
	}

	/**
	 * text display descriptor class.
	 * 
	 */
	public class voSubtitleTextDisplayDescriptor
	{
		/**
		 * get whether enable wrap word
		 * 
		 * @return 0:No Wrap; 1:Wrap
		 */
		public int getWrap() {
			return wrap;
		}
		
		
		/**
		 * get scroll direction
		 * 
		 * @return 0:LEFT_TO_RIGHT; 1:RIGHT_TO_LEFT; 2:TOP_TO_BOTTOM; 3:BOTTOM_TO_TOP
		 */
		public int getScrollDirection() {
			return scrollDirection;
		}
		
		private int					wrap = 0;///<0:No Wrap;1:Wrap
		private int					scrollDirection = 0;///<0:LEFT_TO_RIGHT;1:RIGHT_TO_LEFT;2:TOP_TO_BOTTOM;3:BOTTOM_TO_TOP
		private voSubtitleTextDisplayDescriptor(){}
		private boolean parse(Parcel parc)
		{
	        int type = parc.readInt();
        	if (type != KEY_TEXT_DISPLAY_DESCRIPTOR)
        	{//
        		parc.setDataPosition(parc.dataPosition() - 4);
        		return false;
        	}
        	type = parc.readInt();
        	wrap = parc.readInt();
        	type = parc.readInt();
        	scrollDirection = parc.readInt();
			return true;
		}
	};

	/**
	 * text row descriptor class.
	 * 
	 */
	public class voSubtitleTextRowDescriptor
	{
		/**
		 * 
		 * get the text background rectangle
		 * 
		 * @return the text background rectangle object
		 */
		public voRect getDataBox() {
			return dataBox;
		}
		
		/**
		 * setDataBox
		 * 
		 * @param dataBox the text background rect
		 */
		private void setDataBox(voRect dataBox) {
			this.dataBox = dataBox;
	    	if(settings != null )//&& settings.backgroundColorEnable)  
	    	{
	    		int color = dataBox.rectFillColor;
        		color = settings.converColor(color,settings.backgroundColor,settings.backgroundColorEnable );
        		color = settings.converColorOpacityRate(color, settings.backgroundColorOpacityRate, settings.backgroundColorOpacityRateEnable);
	    		this.dataBox.setRectFillColor(color);
	    	}
		}
		
		/**
		 * get horizontal justification
		 * 
		 * @return 0:Left; 1:Right; 2:Center
		 */
		public int getHorizontalJustification() {
			return horizontalJustification;
		}
		
		/**
		 * get vertical justification
		 * 
		 * @return 0:Top; 1:Bottom; 2:Center
		 */
		public int getVerticalJustification() {
			return verticalJustification;
		}
		
		/**
		 * get print direction
		 * 
		 * @return 0:Left-Right; 1:Right-Left; 2:Top-Bottom; 3:Bottom-Top
		 */
		public int getPrintDirection() {
			return printDirection;
		}
		
		private voRect				dataBox = new voRect();///<the text background box
		private int					horizontalJustification = 0;///<0:Left;1:Right;2:Center
		private int                 verticalJustification = 0;///<0:Top;1:Bottpm;2:Center
		private int					printDirection = 0;///<0:Left-Right;1:Right-Left;2:Top-Bottom;3:Bottom-Top
		private voSubtitleTextRowDescriptor(){}
		private boolean parse(Parcel parc)
		{
	        int type = parc.readInt();
        	if (type != KEY_TEXT_ROW_DESCRIPTOR)
        	{//
        		parc.setDataPosition(parc.dataPosition() - 4);
        		return false;
        	}
        	dataBox = new voRect();
        	if(!dataBox.parse(parc))
        		return false;
        	try
        	{
        	setDataBox(dataBox);
        	}
        	catch(Exception e)
        	{
        		String s = e.toString();
        	}
        	
        	type = parc.readInt();
        	if (type != KEY_TEXT_ROW_DESCRIPTOR_HORI)
        	{//
        		parc.setDataPosition(parc.dataPosition() - 4);
        		return false;
        	}
        	horizontalJustification = parc.readInt();
        	type = parc.readInt();
        	verticalJustification = parc.readInt();
        	type = parc.readInt();
        	printDirection = parc.readInt();
      	
			return true;
		}
	};

	/**
	 * text row information class.
	 * 
	 */
	public class voSubtitleTextRowInfo 
	{
		/**
		 * get the entry about text row information 
		 * 
		 * @return voSubtitleTextInfoEntry array object
		 */
		public ArrayList<voSubtitleTextInfoEntry> getTextInfoEntry() {
			return textInfoEntry;
		}
		
		/**
		 * get text row information descriptor
		 * 
		 * @return voSubtitleTextRowDescriptor object
		 */
		public voSubtitleTextRowDescriptor getTextRowDes() {
			return textRowDes;
		}
		
		private ArrayList<voSubtitleTextInfoEntry>	textInfoEntry = new ArrayList<voSubtitleTextInfoEntry>();
		private voSubtitleTextRowDescriptor		textRowDes = new voSubtitleTextRowDescriptor();
		private voSubtitleTextRowInfo(){}
		private boolean parse(Parcel parc)
		{
        	textInfoEntry = new ArrayList<voSubtitleTextInfoEntry>();
        	
        	boolean bAdd = false;
        	while(true)
        	{
        		voSubtitleTextInfoEntry info = new voSubtitleTextInfoEntry();
	        	if(!info.parse(parc))
	        	{
	        		break;
	        	}
	        	bAdd = true;
	        	textInfoEntry.add(info);
        	}
        	
        	if(bAdd)
        	{
	        	textRowDes = new voSubtitleTextRowDescriptor();
	        	if(!textRowDes.parse(parc))
	        		return false;
				return true;
        	}
        	else
        		return false;
		}

	}//voSubtitleTextRowInfo,*pvoSubtitleTextRowInfo;

	private class voSubtitleImageInfoData
	{
		private VOOSMP_IMAGE_TYPE				imageType;
		private int 							width;
		private int								height;
		private int								size = 0;
		private byte []							picData = null;///<picture data
		
		private voSubtitleImageInfoData(){}
		
		/**
		 * getSize
		 * 
		 * @return the size of image data
		 */
		private int getSize() {
			return size;
		}
		
		/**
		 * getPicData
		 * 
		 * @return the data of image
		 */
		private byte[] getPicData() {
			return picData;
		}
		
		private int getWidth()
		{
			return width;
		}
		
		private int getHeight()
		{
			return height;
		}
		
		private VOOSMP_IMAGE_TYPE getImageType()
		{
			return imageType;
		}
		
		private boolean parse(Parcel parc)
		{
	        int type = parc.readInt();
        	if (type != KEY_IMAGE_TYPE)
        	{//
        		parc.setDataPosition(parc.dataPosition() - 4);
        		return false;
        	}
        	imageType = VOOSMP_IMAGE_TYPE.valueOf(parc.readInt());
        	
        	type = parc.readInt();
        	width = parc.readInt();
        	
        	type = parc.readInt();
        	height = parc.readInt();
        	
        	type = parc.readInt();
        	size = parc.readInt();
        	
        	type = parc.readInt();
        	if(size>0){
	        	picData = new byte[size];
	        	parc.readByteArray(picData);
        	}
			return true;
		}
	}
	
	private class voSubtitleImageInfo
	{
		/**
		 * getImageData
		 * 
		 * @return voSubtitleImageInfoData object
		 */
		private voSubtitleImageInfoData getImageData() {
			return imageData;
		}
		
		/**
		 * getImageInfoDescriptor
		 * 
		 * @return voSubtitleImageInfoDescriptor object
		 */
		private voSubtitleImageInfoDescriptor getImageInfoDescriptor() {
			return imageInfoDescriptor;
		}
		
		private Bitmap getBitmap(){
			if(bitmap!=null)
				return bitmap;
			if(imageData.getPicData() == null)
				return null;
			if (imageData.getImageType() == VOOSMP_IMAGE_TYPE.VOOSMP_IMAGE_RGBA32 ||
					imageData.getImageType() == VOOSMP_IMAGE_TYPE.VOOSMP_IMAGE_ARGB32)
			{
				try
				{
					bitmap = Bitmap.createBitmap(imageData.getWidth(), imageData.getHeight(), 
							Config.ARGB_8888);
					byte bt[] = imageData.getPicData();
					if(imageData.getImageType() == VOOSMP_IMAGE_TYPE.VOOSMP_IMAGE_RGBA32){
						byte bt2[] = new byte[imageData.getPicData().length];
						for(int k=0;k<imageData.getPicData().length;k+=4){
							if(bt[k+3]==0)
							{
								bt2[k]=0;
								bt2[k+1]=0;
								bt2[k+2]=0;
							}
							else
							{
								bt2[k]=bt[k];
								bt2[k+1]=bt[k+1];
								bt2[k+2]=bt[k+2];
								bt2[k+3]=bt[k+3];
							}
						}
						bt = bt2;
					}
					Buffer buffer = ByteBuffer.wrap(bt);
					bitmap.copyPixelsFromBuffer(buffer);
//					if(imageData.getPicData()!=null){
//							FileOutputStream  fos = new FileOutputStream("/mnt/sdcard/dump_dvb.bin");
//							
//							fos.write(imageData.getPicData(), 0, imageData.getPicData().length);
//							fos.flush();
//							fos.close();
//					}
//					voLog.i(TAG, "subtitle image i0 is %d, i1 is %d, i2 is %d. %d : %s",
//							imgInfo.imageData.getPicData()[0],imgInfo.imageData.getPicData()[1],
//							imgInfo.imageData.getPicData()[2],imgInfo.imageData.getPicData()[3],imgInfo.imageData.getImageType() == VOOSMP_IMAGE_TYPE.VOOSMP_IMAGE_RGBA32?"rgba":"argb");
//							imgInfo.imageData.getWidth(), imgInfo.imageData.getHeight(), imgInfo.imageData.getSize() );
				}catch (Exception e)
				{
					voLog.e(TAG, "subtitle image create bitmap failed!");
				}
			}
		    else{
		    	bitmap = BitmapFactory.decodeByteArray(imageData.picData, 0, imageData.picData.length);//decodeStream(bis);
		    }
			return bitmap;
		}
		private voSubtitleImageInfoData			imageData = null;
		private voSubtitleImageInfoDescriptor	imageInfoDescriptor = null;
		private Bitmap bitmap = null;
		private voSubtitleImageInfo(){}
		private boolean parse(Parcel parc)
		{
	        int type = parc.readInt();
        	if (type != KEY_IMAGE_INFO)
        	{//
        		parc.setDataPosition(parc.dataPosition() - 4);
        		return false;
        	}
        	imageData = new voSubtitleImageInfoData();
        	imageData.parse(parc);
       		imageInfoDescriptor = new voSubtitleImageInfoDescriptor();
        	imageInfoDescriptor.parse(parc);
 			return true;
		}
	}//voSubtitleImageInfo,pvoSubtitleImageInfo;

	private class voSubtitleImageInfoDescriptor
	{
		/**
		 * getImageRectInfo
		 * 
		 * @return voRect object
		 */
		private voRect getImageRectInfo() {
			return (voRect)imageRectInfo;
		}
		private voRect	imageRectInfo = null;
		private voSubtitleImageInfoDescriptor(){}
		private boolean parse(Parcel parc)
		{
	        int type = parc.readInt();
        	if (type != KEY_IMAGE_DISPLAY_DESCRIPTOR)
        	{//
        		parc.setDataPosition(parc.dataPosition() - 4);
        		return false;
        	}
        	imageRectInfo = new voRect();
        	if(!imageRectInfo.parse(parc))
        		return false;
			return true;
		}
	};
	
	/**
	 * display information class.
	 * 
	 */
	public class voSubtitleDisplayInfo
	{
		/**
		 * get text row object array 
		 * 
		 * @return voSubtitleTextRowInfo object array
		 */
		public ArrayList<voSubtitleTextRowInfo> getTextRowInfo() {
			return textRowInfo;
		}
		/**
		 * get display descriptor
		 * 
		 * @return voSubtitleTextDisplayDescriptor object
		 */
		public voSubtitleTextDisplayDescriptor getDispDescriptor() {
			return dispDescriptor;
		}
		/**
		 * getImageInfo
		 * 
		 * @return voSubtitleImageInfo array object
		 */
		private ArrayList<voSubtitleImageInfo> getImageInfo() {
			return imageInfo;
		}
		private ArrayList<voSubtitleTextRowInfo>	textRowInfo = new ArrayList<voSubtitleTextRowInfo>();
		private voSubtitleTextDisplayDescriptor		dispDescriptor = new voSubtitleTextDisplayDescriptor();///<describe the common style used to all voSubTitleTextInfoEntry
		private ArrayList<voSubtitleImageInfo>		imageInfo = new ArrayList<voSubtitleImageInfo>();
		private voSubtitleDisplayInfo(){}
		private boolean parse(Parcel parc)
		{
			//parc.m
	        int type = parc.readInt();
        	if (type != KEY_DISPLAY_INFO)
        	{
        		parc.setDataPosition(parc.dataPosition() - 4);
        		return false;
        	}
 	        type = parc.readInt();
        	if (type != KEY_TEXT_ROW_INFO)
        	{
        		parc.setDataPosition(parc.dataPosition() - 4);
        	}
        	else
        	{
	        	textRowInfo = new ArrayList<voSubtitleTextRowInfo>();
	        	
	        	while(true)
	        	{
		        	voSubtitleTextRowInfo info = new voSubtitleTextRowInfo();
		        	if(!info.parse(parc))
		        		break;
		        	if(settings != null)  
		        	{
		        		if(settings.rectList!=null)
		        		{
		        			if(settings.rectList.size()>textRowInfo.size())
		        			{
		        				Rect rt = settings.rectList.get(textRowInfo.size());
		        				if(info.textRowDes!=null)
		        				{
		        					if(info.textRowDes.dataBox!=null)
		        						info.textRowDes.dataBox.rectDraw = rt;
		        				}
		        			}
		        		}
		        	}
		        	textRowInfo.add(info);
	        	}
        	}
        	
        	dispDescriptor = new voSubtitleTextDisplayDescriptor();
        	if(!dispDescriptor.parse(parc))
        		return false;
	        type = parc.readInt();
    		parc.setDataPosition(parc.dataPosition() - 4);
        	if (type != KEY_IMAGE_INFO)
        	{//if voSubtitleImageInfo is null
        		return true;
        	}
        	imageInfo =  new ArrayList<voSubtitleImageInfo>();//new voSubtitleImageInfo();
        	while(true)
        	{
        		if(parc.dataAvail()<=0)
        			break;
        		voSubtitleImageInfo info = new voSubtitleImageInfo();
	        	if(!info.parse(parc))
	        		break;
	        	imageInfo.add(info);
        	}
       	
			return true;
		}
	};

	/**
	 * closed caption information entry class.
	 * 
	 */
	public class voSubtitleInfoEntry 
	{	
		/**
		 * get duration
		 * 
		 * @return the duration of the subtitle
		 */
		public int getDuration() {
			return duration;
		}

		/**
		 * get rectangle Information
		 * 
		 * @return voRect object
		 */
		public voRect getSubtitleRectInfo() {
			return (voRect)subtitleRectInfo;
		}

		/**
		 * get display information
		 * 
		 * @return voSubtitleDisplayInfo object
		 */
		public voSubtitleDisplayInfo getSubtitleDispInfo() {
			return subtitleDispInfo;
		}
		

		private int						duration = 0xFFFFFFFF;	///<0xFFFFFFFF indicate no duration information
		private voRect					subtitleRectInfo = new voRect(); ///< information of Rect which text in  
		private voSubtitleDisplayInfo	subtitleDispInfo = new voSubtitleDisplayInfo(); ///<all the text && picture info
		private ArrayList<View>			mChildViewArr = new ArrayList<View>();
		private voSubtitleInfoEntry(){}
		
		private boolean parse(Parcel parc)
		{
	        int type = parc.readInt();
        	if (type != KEY_DURATION)
        	{
	        	if (parc.dataAvail()<=0)
	           		return false;
        		parc.setDataPosition(parc.dataPosition() - 4);
        		return false;
        	}
        	duration = parc.readInt();
	        subtitleRectInfo = new voRect();
	        if(!subtitleRectInfo.parse(parc))
	        	return false;
	        subtitleDispInfo = new voSubtitleDisplayInfo();
	        if(!subtitleDispInfo.parse(parc))
	        	return false;
			return true;
		}
	}//voSubtitleInfoEntry,*pvoSubtitleInfoEntry;

	/**
	 * closed caption information class.
	 * 
	 */
	public class voSubtitleInfo
	{
		/**
		 * get time stamp
		 * 
		 * @return time stamp
		 */
		public int getTimeStamp() {
			return timeStamp;
		}
		/**
		 * getMaxDuration
		 * 
		 * @return max duration
		 */
		private int getMaxDuration() {
	        if(subtitleEntry == null)
	        	return 0;
	        else
			{
				for(int i = 0; i<subtitleEntry.size(); i++)
				{
					voSubtitleInfoEntry info = subtitleEntry.get(i);
					if(info.duration==0xffffffff)// || timeStamp+info.duration>curTime) && (timeStamp<curTime))
					{
						maxDuration = -1;
						break;
					}
					if(info.duration>maxDuration)
						maxDuration = info.duration;
				}
			}
			return maxDuration;
		}
		/**
		 * get entry object array 
		 * 
		 * @return voSubtitleInfoEntry object array
		 */
		public ArrayList<voSubtitleInfoEntry> getSubtitleEntry() {
			return subtitleEntry;
		}
		/**
		 * sort
		 * 
		 * @return void
		 */
		public void sortByZIndex(){
			if(subtitleEntry == null)
				return;
			Collections.sort(subtitleEntry, new SortByZOrder());
		}
		class SortByZOrder implements Comparator {
			 public int compare(Object o1, Object o2) {
				 voSubtitleInfoEntry s1 = (voSubtitleInfoEntry) o1;
				 voSubtitleInfoEntry s2 = (voSubtitleInfoEntry) o2;
				 if (s1.getSubtitleRectInfo().rectZOrder > s2.getSubtitleRectInfo().rectZOrder)
				   return 1;
				 return 0;
			 }
		}

		
		private int									timeStamp = 0;
		private int									maxDuration = -1;
		private ArrayList<voSubtitleInfoEntry>		subtitleEntry = new ArrayList<voSubtitleInfoEntry>();
		private voSubtitleInfo(){}
		private boolean parse(Parcel parc)
		{
	        int type = parc.readInt();
	        if (type == KEY_START_TIME) {
	        	timeStamp = parc.readInt();
	        	type = parc.readInt();
	        	if (type != KEY_SUBTITLE_INFO)
	        		return false;
	        	while(true)
	        	{
		        	if (parc.dataAvail()<=0)
		                break;
		        	
		        	voSubtitleInfoEntry infoEntry = new voSubtitleInfoEntry();
		        	if(infoEntry.parse(parc))
		        	{
			        	if(subtitleEntry == null)
			        		subtitleEntry = new ArrayList<voSubtitleInfoEntry>();
			        	subtitleEntry.add(infoEntry);
		        	}
		        	else
		        		break;
	        	}
	       	
	        } else  {
	            //Log.w(TAG, "Invalid timed text key found: " + type);
	            return false;
	        }
	        
	        //to get max duration
	        if(subtitleEntry == null)
	        	return true;
	        else
			{
	        	getMaxDuration();
//				for(int i = 0; i<subtitleEntry.size(); i++)
//				{
//					voSubtitleInfoEntry info = subtitleEntry.get(i);
//					if(info.duration==0xffffffff)// || timeStamp+info.duration>curTime) && (timeStamp<curTime))
//					{
//						maxDuration = -1;
//						break;
//					}
//					if(info.duration>maxDuration)
//						maxDuration = info.duration;
//				}
			}
			
			return true;
		}
	}//voSubtitleInfo;	
	private class LinearLayoutShowBorder extends LinearLayout {
		
		int m_nWidth = 0;
		int m_nColor = 0;
		Paint paintBorder = new Paint();

		public LinearLayoutShowBorder(Context context) {
			super(context);
		}

		public LinearLayoutShowBorder(Context context, AttributeSet attrs) {
			super(context, attrs);
		}
		
		public void setBorder(int nWidth,int nColor){
			m_nWidth =nWidth;
			m_nColor = nColor;
			paintBorder.setStrokeWidth(nWidth);
			paintBorder.setColor(/*Color.BLACK*/nColor);
		}
	 

		@Override
		protected void onDraw(Canvas canvas) {
			super.onDraw(canvas);
			
			if(m_nWidth == 0) return;
			
	        canvas.drawLine(0, 0, this.getWidth(), 0, paintBorder);
	        canvas.drawLine(0, 0, 0, this.getHeight(), paintBorder);
	        canvas.drawLine(this.getWidth(), 0, this.getWidth(), this.getHeight(), paintBorder);
	        canvas.drawLine(0, this.getHeight(), this.getWidth(), this.getHeight(), paintBorder);
		}
	}
	private class TextOutLinesView extends TextView {   
	  
		private boolean enableDraw = true;
		private Canvas canvas2 = null;
		private Bitmap bitmap = null;

		public void enableDraw(boolean enableD)
		{
			enableDraw = enableD;
			//this.invalidate();
			this.setVisibility(enableD?View.VISIBLE:View.GONE);
		}
	    //Constructor - for xml   
	    public TextOutLinesView(Context context, AttributeSet attrs) {   
	        super(context, attrs);   
	    }   
	    public TextOutLinesView(Context context) {   
	        super(context);   
	    }   
	    
	    private int getItalic(int def){
	    	int n = def;
			if(settings!=null && settings.fontItalicEnable){
				n = settings.fontItalic;
			}
	    	return n;
	    }
	    private int getEdgeType(int def){
	    	int n = def;
			if(settings!=null && settings.edgeTypeEnable){
				n = settings.edgeType;
			}
	    	return n;
	    }
	    private int getEdgeColor(int def){
	    	int n = def;
			if(settings!=null){
	    		if(settings!=null){
	        		n = settings.converColor(n,settings.edgeColor,settings.edgeColorEnable );
	        		n = settings.converColorOpacityRate(n, settings.edgeColorOpacityRate, settings.edgeColorOpacityRateEnable);
	    		}
			}
	    	return n;
	    }
	    private int setTextViewTextInfo(voSubtitleTextRowInfo rowInfo, voSubtitleTextInfoEntry textInfo, Paint textPaint, Paint strokePaint)
	    {
	    	int nFntSize = 0;
    		int color = textInfo.stringInfo.fontInfo.fontColor;
    		if(settings!=null){
        		color = settings.converColor(color,settings.fontColor,settings.fontColorEnable );
        		color = settings.converColorOpacityRate(color, settings.fontColorOpacityRate, settings.fontColorOpacityRateEnable);
    		}
	    	textPaint.setColor(color);
	    	int tpFont = Typeface.NORMAL;
	    	if(textInfo.stringInfo.fontInfo.fontSize == 103)//to support ttml
	    		tpFont = Typeface.NORMAL;
    		if((textInfo.stringInfo.fontInfo.fontSize == 102))
    			tpFont = Typeface.BOLD;
	    	if(settings != null && settings.fontBoldEnable)  
	    	{
	    		tpFont = settings.fontBold;
	    	}
    		
	    	int italic = getItalic(textInfo.stringInfo.charEffect.Italic);
			if(italic!=0){
				tpFont = Typeface.ITALIC|tpFont;
			}
			//voLog.v("CloseCaption","font type CloseCaption = %d ", (int)tpFont); 
			Typeface fontTf =Typeface.create(fromID(textInfo.stringInfo.fontInfo.fontStyle), tpFont);

	    	if(settings != null )//&& settings.fontName != null)  
	    	{
	    		if(settings.fontTypeface !=null){
	    			if(settings.fontName.compareTo("")!=0)
	    				fontTf = Typeface.create(settings.fontTypeface, tpFont);
	    			else
	    				fontTf = settings.fontTypeface;
	    		}
	    		else if(settings.fontName !=null){
		    		if(settings.fontName.compareTo("")!=0)
		    		{
		    			fontTf = Typeface.create(settings.fontName,tpFont);
		    		}
	    		}
	    			
	    	}
	    	textPaint.setFakeBoldText(tpFont == Typeface.BOLD || tpFont == Typeface.BOLD_ITALIC);
	    	if(tpFont == Typeface.ITALIC || tpFont == Typeface.BOLD_ITALIC)
	    	{
		    	textPaint.setTextSkewX(-0.5f);
	    	}

			textPaint.setTypeface(fontTf);		
			if(strokePaint!=null)
			{
				strokePaint.setTypeface(fontTf);		
				strokePaint.setFakeBoldText(tpFont == Typeface.BOLD || tpFont == Typeface.BOLD_ITALIC);	
				if(tpFont == Typeface.ITALIC || tpFont == Typeface.BOLD_ITALIC)
					strokePaint.setTextSkewX(-0.5f);
			}
			{
				boolean bVertical = rowInfo.textRowDes.printDirection>1?true:false;
				
				float rate2 = getFontSize(textInfo);
				float fsz2 = 0;
				if(bVertical)
				{
					int nLeft = toRealX(rowInfo.textRowDes.dataBox.rectDraw.left);
					int nRight = toRealX(rowInfo.textRowDes.dataBox.rectDraw.right);
					fsz2 = ((nRight-nLeft)*rate2);
					textPaint.setTextSize(fsz2);
					if(strokePaint!=null) strokePaint.setTextSize(fsz2);
				}
				else
				{
					int nTop = toRealY(rowInfo.textRowDes.dataBox.rectDraw.top);
					int nBottom = toRealY(rowInfo.textRowDes.dataBox.rectDraw.bottom);
					fsz2 = (float)((nBottom-nTop)*rate2);//tv.getTextSize();
					textPaint.setTextSize(fsz2);
					if(strokePaint!=null) strokePaint.setTextSize(fsz2);
					
					//voLog.v("CloseCaption","font size CloseCaption = %d , %.2f", (int)fsz2, density); 
				}
				
				nFntSize = (int)(fsz2);
			}
			
			int underline = textInfo.stringInfo.charEffect.Underline;
			if(settings!=null && settings.fontUnderlineEnable){
				underline = settings.fontUnderline;
			}
			if(underline!=0){
				textPaint.setUnderlineText(true);
				//if(strokePaint!=null) strokePaint.setUnderlineText(true);
			}
			int edgeType =getEdgeType( textInfo.stringInfo.charEffect.EdgeType);
			if(edgeType!=0){
	    		color =  getEdgeColor(textInfo.stringInfo.charEffect.EdgeColor);

				float radius = 1.8f;
				if(nFntSize/9.0f>radius)
					radius = nFntSize/9.0f;
				if(edgeType == 1 || edgeType == 2 || edgeType == 3)
				{	
					if(strokePaint!=null) strokePaint.setColor(color);
					if( edgeType == 3)
						textPaint.setShadowLayer(radius, 0, 0, color);
				}
				if(edgeType == 4)
				{
					textPaint.setShadowLayer(radius, -2, 2, color);
				}
				if( edgeType == 5)
				{
					textPaint.setShadowLayer(radius, 2, 2, color);
				}
				//int	EdgeType=0;///<0:NONE,1:RAISED,2:DEPRESSED,3:UNIFORM,4:LEFT_DROP_SHADOW,5:RIGHT_DROP_SHADOW
			}
			return nFntSize;
	    }
	    

	    private void drawPicture(Canvas canvas, voSubtitleDisplayInfo dispInfo){
	    	if(dispInfo.imageInfo == null || metadata_Arrive || width <=0 || height <= 0)
	    		return;
			for(int j = 0; j < dispInfo.imageInfo.size() ; j++)
			{
				voSubtitleImageInfo imgInfo = dispInfo.imageInfo.get(j);
				if( imgInfo == null)
					continue;
				if( imgInfo.getBitmap() == null)
					continue;
				
				int nLeft = -1;
				int nRight = -1;
				int nTop = -1;
				int nBottom = -1;
				if( imgInfo.imageInfoDescriptor != null){
					if(imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.left>=0)	nLeft = toRealX(imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.left);
					if(imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.right>=0)	nRight = toRealX(imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.right);
					if(imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.top>=0)	nTop = toRealY(imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.top);
					if(imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.bottom>=0)	nBottom = toRealY(imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.bottom);
//					voLog.i(TAG, "Rect left is %d, top is %d, right is %d, bottom is %d. \nReal rect left is %d, top is %d, right is %d, bottom is %d.",
//							imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.left, imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.top, 
//							imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.right, imgInfo.imageInfoDescriptor.imageRectInfo.rectDraw.bottom,
//							nLeft, nTop, nRight, nBottom);
				}

				int nHeightBmp =	imgInfo.getBitmap().getHeight();
				int nWidthBmp = imgInfo.getBitmap().getWidth();
				if(nHeightBmp<=0 || nWidthBmp<=0)
					continue;

				Rect dst = new Rect();
				if((nLeft<=0 && nRight<=0))
				{
					int nWidthDraw = nWidthBmp;
					int nHeightDraw = nHeightBmp;
					if(width<nWidthBmp || height<nHeightBmp){
						if(nWidthBmp/(float)nHeightBmp>xyRate)
						{
							nHeightDraw = nWidthDraw*nHeightBmp/nWidthBmp;
						}
						else{
							nWidthDraw = nHeightDraw*nWidthBmp/nHeightBmp;
						}
					}
					dst.top = (height-nHeightDraw)/2;
					dst.left = (width-nWidthDraw)/2;
					dst.bottom = dst.top+nHeightDraw;
					dst.right = dst.left+nWidthDraw;
				}
				else if(nRight==-1)//to draw picture with its width
				{
					dst.top = nTop;
					dst.left = nLeft;
					dst.bottom = dst.top+nHeightBmp;
					dst.right = dst.left+nWidthBmp;
				}
				else
				{
					dst.top = nTop;
					dst.left = nLeft;
					dst.bottom = nBottom;
					dst.right = nRight;
				}
				Paint paint = new Paint(); 
				paint.setAntiAlias(true);
				canvas.setDrawFilter(new PaintFlagsDrawFilter(0,Paint.ANTI_ALIAS_FLAG|Paint.FILTER_BITMAP_FLAG));
				canvas.drawBitmap(imgInfo.getBitmap(), null, dst, paint);

			}
	    	
	    }

	    @Override 
	    public void onDraw(Canvas canvas) { 
	    	if(!enableDraw)
	    	{
		    	return;
	    	}
	    	if(mIsPreview){
	    		width = canvas.getWidth();
	    		height = canvas.getHeight();
	    		if(vwSurface!=null){
	    			width = vwSurface.getWidth();
	    			height = vwSurface.getHeight();
	    		}
				if(height>0)
					xyRate = width/(float)height;
				else{
		    		width = 600;
		    		height = 90;
		    		xyRate = width/(float)height;
				}
	    	}
			if(mainActivity == null || mSubtitleInfo == null) return;
			if(mSubtitleInfo.subtitleEntry == null) return;
	    	
			voSubtitleInfo subtitleInfo = mSubtitleInfo;
	    	boolean bScaled = false;
	    	
	    	if(canvas2 == null)
	    		canvas2 = new Canvas();
    		bitmap = Bitmap.createBitmap(canvas.getWidth(), canvas.getHeight()*3/2, Bitmap.Config.ARGB_8888);
	    	canvas2.setBitmap(bitmap);
	    	canvas2.drawARGB(0, 0, 0, 0);
	    	
	    	ArrayList<Rect> arrRect = new ArrayList<Rect>();
			for(int i = 0; i<subtitleInfo.subtitleEntry.size(); i++)
			{
				Rect rtEntry = new Rect();
				arrRect.add(rtEntry);
				rtEntry.left = -1;
				rtEntry.right = -1;
				rtEntry.top = -1;
				rtEntry.bottom = -1;
				
				voSubtitleInfoEntry info = subtitleInfo.subtitleEntry.get(i);
				voSubtitleDisplayInfo dispInfo = info.subtitleDispInfo;
				
		    	drawPicture(canvas2, dispInfo);
		    	
				if(dispInfo.textRowInfo != null)
				{
					int nOldBottom = -1;
					//int nCurrPos = -1;
					//voLog.i(TAG, "getFontSize textRowInfo = %d",dispInfo.textRowInfo.size());
					for(int j = 0; j < dispInfo.textRowInfo.size() ; j++)
					{
						voSubtitleTextRowInfo rowInfo = dispInfo.textRowInfo.get(j);
						if( rowInfo == null)
							continue;
						if( rowInfo.textInfoEntry == null)
							continue;
						
						int nLeft = toRealX(rowInfo.textRowDes.dataBox.rectDraw.left);
						int nRight = toRealX(rowInfo.textRowDes.dataBox.rectDraw.right);
						if(nRight<nLeft){
							int tmp = nLeft;
							nRight = nLeft;
							nLeft = tmp;
						}
						
						int nTop = toRealY(rowInfo.textRowDes.dataBox.rectDraw.top);
						int nBottom = toRealY(rowInfo.textRowDes.dataBox.rectDraw.bottom);//nRight-nLeft,nBottom-nTop);//
						if(nOldBottom>=0){
							if(nTop<nOldBottom){
								int nRowGap = nOldBottom- nTop;
								nTop += nRowGap;
								nBottom += nRowGap;
							}
						}
						if(rtEntry.left == -1){
							rtEntry.left = nLeft;
						}else if(rtEntry.left > nLeft){
							rtEntry.left = nLeft;
						}
						if(rtEntry.top == -1){
							rtEntry.top = nTop;
						}else if(rtEntry.top > nTop){
							rtEntry.top = nTop;
						}

						int nHeightReal = nBottom-nTop;

						int nFontMaxSize = 0;
						for(int k = 0; k < rowInfo.textInfoEntry.size() ; k++){
							voSubtitleTextInfoEntry textInfo = rowInfo.textInfoEntry.get(k);
							if(textInfo.stringInfo.fontInfo.fontSize>1000 && textInfo.stringInfo.fontInfo.fontSize<10000){
								if(textInfo.stringInfo.fontInfo.fontSize-1000>nFontMaxSize)
									nFontMaxSize = textInfo.stringInfo.fontInfo.fontSize-1000;
							}
							
						}
						if(nFontMaxSize>0){
							float f = (float)nFontMaxSize/100.0f;
							nHeightReal = (int)(nHeightReal*f);
						}
						if(settings != null && settings.fontSizeEnable){
							nHeightReal = (int)(nHeightReal*settings.fontSizeMedium);
						}
						nBottom = nTop+nHeightReal;
						
						//voLog.i(TAG, "getFontSize textInfoEntry = %d",rowInfo.textInfoEntry.size());
						int nLeftFirst = nLeft;
						int nLengthALine = 0;
						for(int k = 0; k < rowInfo.textInfoEntry.size() ; k++)
						{
					    	Paint textPaint = new Paint();     
							textPaint.setAntiAlias(true);
							voSubtitleTextInfoEntry textInfo = rowInfo.textInfoEntry.get(k);
					    	Paint strokePaint = null; 
					    	String strText =textInfo.stringText;
							setTextViewTextInfo(rowInfo, textInfo, textPaint,strokePaint);
						    textPaint.setTextAlign(Paint.Align.LEFT);    
							nLengthALine+=(int)textPaint.measureText(strText);
						}
						if(rowInfo.textRowDes.horizontalJustification==1 
									||rowInfo.textRowDes.horizontalJustification==2){
							if(rowInfo.textRowDes.horizontalJustification==1){
								nLeft = (nRight-nLengthALine);
								if(nLeft<2)
									nLeft = 2;
								nLeftFirst = nLeft;
								
							}
							if(rowInfo.textRowDes.horizontalJustification==2){
								nLeft = (width-nLengthALine)/2;
								if(nLeft<2)
									nLeft = 2;
								nLeftFirst = nLeft;
								
							}
						}
						for(int k = 0; k < rowInfo.textInfoEntry.size() ; k++)
						{
							Paint paintBack = new Paint(); 
							paintBack.setTextAlign(Paint.Align.LEFT);   
				    		int color = rowInfo.textRowDes.dataBox.rectFillColor;
				    		if(settings!=null){
				        		color = settings.converColor(color,settings.backgroundColor,settings.backgroundColorEnable );
				        		color = settings.converColorOpacityRate(color, settings.backgroundColorOpacityRate, settings.backgroundColorOpacityRateEnable);
				    		}
							paintBack.setColor(color);
							if(paintBack.getAlpha() == 0)
								paintBack = null;
					    	Paint textPaint = new Paint();     
							textPaint.setAntiAlias(true);
							voSubtitleTextInfoEntry textInfo = rowInfo.textInfoEntry.get(k);
							
					    	Paint strokePaint = null; 
							int edgeType =getEdgeType( textInfo.stringInfo.charEffect.EdgeType);

					    	if(edgeType< 1 || edgeType > 3 )
					    	{
					    		strokePaint = null;
					    	}
					    	else
					    	{
					    		strokePaint = new Paint();
					    		strokePaint.setAntiAlias(true);
					    		strokePaint.setTextAlign(Paint.Align.LEFT);    
					    	}
							//to draw now
							{
						    	String strText =textInfo.stringText;
								boolean bVertical = rowInfo.textRowDes.printDirection>1?true:false;
								if(rowInfo.textRowDes.printDirection == 1 || rowInfo.textRowDes.printDirection == 3 )
									strText = reverseString(strText);
								//voLog.i(TAG, "getFontSize strText = %s",strText);
								int nFntSize = setTextViewTextInfo(rowInfo, textInfo, textPaint,strokePaint);
								if(strokePaint!=null)
								{
									strokePaint.setTextAlign(Paint.Align.LEFT); 
							    	strokePaint.setStyle(Paint.Style.STROKE);     
							    	strokePaint.setStrokeWidth(4);    
								}
							    textPaint.setTextAlign(Paint.Align.LEFT);    
								int desc = (int)textPaint.descent()+1;
								//voLog.v("CloseCaption","desc 1111 CloseCaption = %d ",desc); 

							    Rect rtBack = new Rect();
							    int italic = getItalic(textInfo.stringInfo.charEffect.Italic);
						    	if(bVertical)
						    	{
									int nLen = strText.length();
									
									if(nFntSize<nRight -nLeft)
									{
										if(((nRight -nLeft) - nFntSize)>0)
											desc += ((nRight -nLeft) - nFntSize)/3;
									}
									
							    	rtBack.left = nLeft-3;
							    	int rightAdd = 6;
							    	if(italic!=0)
							    		rightAdd+=nFntSize/2;
									int width = (nRight -nLeft);
									for(int i_char = 0; i_char < nLen; i_char ++){
										char szChar = strText.charAt(i_char);
										String str = "";
										str += szChar;
										//textPaint.s(rowInfo.textRowDes.dataBox.rectFillColor);
										if(paintBack!=null)
										{
									    	rtBack.top = nTop+width * (i_char);
									    	rtBack.bottom = nTop+width * (i_char + 1);
									    	rtBack.right = rtBack.left+(int)textPaint.measureText(str)+rightAdd;
											canvas2.drawRect(rtBack, paintBack);
										}
								    	if(strokePaint!=null )
								    		canvas2.drawText(str, nLeft+2, nTop+(width) * (i_char + 1)-width/4, strokePaint);      
								    	canvas2.drawText(str, nLeft+2, nTop+(width) * (i_char + 1)-width/4, textPaint);      
									}
									nTop+=width*nLen;
						    	}
						    	else
						    	{
									if(nFntSize<nBottom -nTop)
									{
										if(((nBottom -nTop) - nFntSize)>0)
											desc += ((nBottom -nTop) - nFntSize)/3;
									}
							    	String [] text = strText.split("\n");
							    	ArrayList arr = new ArrayList<String>();
							    	for (int  j1 = 0; j1 < text.length; j1++)
							    	{
							    		if(text[j1].length()>1)
							    		{
							    			arr.add(text[j1]);
							    		}
							    		else if(text[j1].length()==1)
							    		{//in some case, we need add it to tail of last item
							    			if(arr.size()>0){
							    				String s = (String)arr.get(arr.size() -1);
							    				if(s.length()<32){
							    					s+=text[j1];
							    					arr.set(arr.size() -1, s);
							    				}
							    				else
							    					arr.add(text[j1]);
							    			}
							    			else
							    				arr.add(text[j1]);
							    		}
							    	}
							    	
							    	int leftNow = nLeft;

							    	int nWidText = 0;
							    	String strLeft = "";
							    	for (int  j1 = 0; j1 < arr.size(); j1++)
							    	{
							    		String sUse = (String)arr.get(j1);
										//voLog.v("CloseCaption","111 drawText = %s ; sUse.length() = %d",sUse,sUse.length()); 
							    		if(sUse.length()<=0)
							    			continue;
							    		strLeft = sUse;
						    		
							    		boolean bNextLine = false;
							    		while(strLeft.length()>0){
								    		//strLeft = sUse;
								    		nWidText = (int)textPaint.measureText(strLeft);
								    		boolean bNeedSplit = nWidText+leftNow>width;
								    		if(bNeedSplit){//need split row
								    			int nPos = textPaint.breakText(strLeft,true,width - leftNow,null);
								    			if(nPos>=0){
								    				sUse = strLeft.substring(0,nPos);
													int newLine = sUse.lastIndexOf(" ");
													if(newLine == -1){
														newLine = sUse.lastIndexOf("\t");
													}
													if((newLine >0) && (nPos-newLine<10)){
														strLeft = strLeft.substring(newLine);
														sUse = sUse.substring(0,newLine);
													}
													else
														strLeft = strLeft.substring(nPos);
								    				bNextLine = true;
								    			}
								    			else{
									    			sUse = strLeft;
									    			strLeft = "";
									    			bNextLine = false;
								    				
								    			}
							    				nWidText = (int)textPaint.measureText(sUse);
								    			
								    		}
								    		else{
								    			sUse = strLeft;
								    			strLeft = "";
								    			bNextLine = false;
								    		}
									    	rtBack.left = leftNow;
									    	int rightAdd = 5;
									    	if(k == 0 || nLeftFirst == leftNow)
									    	{
									    		rtBack.left -= 5;
									    		rightAdd+=5;
									    	}
									    	if(italic!=0)
									    		rightAdd+=nFntSize/2;
									    	
											//voLog.v("Subtitle","Subtitle = %s ",text[j1]); 
											{
										    	rtBack.top = nTop+(nBottom -nTop) * (j1);
										    	rtBack.bottom = nTop+(nBottom -nTop) * (j1 + 1);
										    	rtBack.right = rtBack.left+nWidText+rightAdd;
										    	if(bScaled)
										    		rtBack.right+=rightAdd;
												if(paintBack!=null)
													canvas2.drawRect(rtBack, paintBack);
											}
									    	if(strokePaint!=null )
									    	{
									   		 //* @return 0:NONE, 1:RAISED, 2:DEPRESSED, 3:UNIFORM, 4:LEFT_DROP_SHADOW, 5:RIGHT_DROP_SHADOW
									    		int topAdd = 0;
									    		if(edgeType > 2)
									    		{
										    		canvas2.drawText(sUse, leftNow, nTop+(nBottom -nTop) * (j1 + 1)-desc+topAdd, strokePaint);
									    		}
									    		else{
									    			int old = textPaint.getColor();
									    			textPaint.setColor(getEdgeColor(textInfo.stringInfo.charEffect.EdgeColor));
									    			if(edgeType == 2)
									    				canvas2.drawText(sUse, leftNow+2, nTop+(nBottom -nTop) * (j1 + 1)-desc+2, textPaint);
									    			if(edgeType == 1)
									    				canvas2.drawText(sUse, leftNow-2, nTop+(nBottom -nTop) * (j1 + 1)-desc-2, textPaint);
									    			textPaint.setColor(old);
									    		}
									    	}
									    	canvas2.drawText(sUse, leftNow, nTop+(nBottom -nTop) * (j1 + 1)-desc, textPaint);
											if(rtEntry.right == -1){
												rtEntry.right = rtBack.right;
											}else if(rtEntry.right < rtBack.right){
												rtEntry.right = rtBack.right;
											}
											if(rtEntry.bottom == -1){
												rtEntry.bottom = rtBack.bottom;
											}else if(rtEntry.bottom < rtBack.bottom){
												rtEntry.bottom = rtBack.bottom;
											}
											
									    	if(rowInfo.textInfoEntry.size()>1 && text.length==1){
									    		nLeft+=nWidText;
										    if(italic!=0)
										    	nLeft+=nFntSize/3;
									    	}
									    	
						    				if(bNextLine){
							    				nLeft = nLeftFirst;
							    				leftNow = nLeftFirst;
							    				int nH = nBottom -nTop;
							    				nTop = rtBack.bottom;
							    				nBottom+=nH;
						    				}

											//voLog.v("CloseCaption","drawText =  ;left=%d, right = %d, horJust = %d, size = %d",rowInfo.textRowDes.dataBox.rectDraw.left,rowInfo.textRowDes.dataBox.rectDraw.right,rowInfo.textRowDes.horizontalJustification, rowInfo.textInfoEntry.size() );
							    		}
							    	}
						    	}
						
							}
						}
						nOldBottom = rtEntry.bottom;
						
					}
				}
			}
			
			int maxBottomMove = 0;
			if(arrRect.size()>0){
				int nMax = arrRect.get(arrRect.size()-1).bottom;
				if(nMax>height){
					maxBottomMove = height - nMax;
				}
			}
			//to draw window background
			for(int i = 0; i<subtitleInfo.subtitleEntry.size(); i++)
			{
				voSubtitleInfoEntry info = subtitleInfo.subtitleEntry.get(i);
				voSubtitleDisplayInfo dispInfo = info.subtitleDispInfo;
				
				boolean windowDraw = false;
				//to draw background
		    	{
					Paint paint = new Paint(); 
					paint.setTextAlign(Paint.Align.LEFT);    
		    		int color = info.subtitleRectInfo.rectFillColor;
			    	if(settings != null)  
			    	{
		        		color = settings.converColor(color,settings.windowBackgroundColor,settings.windowBackgroundColorEnable );
		        		color = settings.converColorOpacityRate(color, settings.windowBackgroundColorOpacityRate, settings.windowBackgroundColorOpacityRateEnable);
			    	}
		    		paint.setColor(color);
			    	boolean bHasText = false;
					if(paint.getAlpha() != 0)
					{
						int nLeft = -1;
						int nRight = -1;
						int nTop = -1;
						int nBottom = -1;//nRight-nLeft,nBottom-nTop);//
						if(dispInfo.textRowInfo != null)
						{
							for(int j = 0; j < dispInfo.textRowInfo.size() ; j++)
							{
								voSubtitleTextRowInfo rowInfo = dispInfo.textRowInfo.get(j);
								if( rowInfo == null)
									continue;
								if( rowInfo.textInfoEntry == null)
									continue;
								if(rowInfo.textRowDes.dataBox.rectDraw.left<nLeft || nLeft ==-1)
									nLeft = rowInfo.textRowDes.dataBox.rectDraw.left;
								if(rowInfo.textRowDes.dataBox.rectDraw.top<nTop || nTop ==-1)
									nTop = rowInfo.textRowDes.dataBox.rectDraw.top;
								if(rowInfo.textRowDes.dataBox.rectDraw.right>nRight || nRight ==-1)
									nRight = rowInfo.textRowDes.dataBox.rectDraw.right;
								if(rowInfo.textRowDes.dataBox.rectDraw.bottom>nBottom || nBottom ==-1)
									nBottom = rowInfo.textRowDes.dataBox.rectDraw.bottom;
								for(int k = 0;k<rowInfo.textInfoEntry.size();k++)
								{
									if(rowInfo.textInfoEntry.get(k)!=null)
									{
										if(rowInfo.textInfoEntry.get(k).stringText!=null)
											if(rowInfo.textInfoEntry.get(k).stringText.length()>0)
												bHasText = true;
									}
								}
							}
						}
						if(bHasText && !(info.subtitleRectInfo.rectDraw.left == -1
								&& info.subtitleRectInfo.rectDraw.right == -1
								&& info.subtitleRectInfo.rectDraw.top == -1
								&& info.subtitleRectInfo.rectDraw.bottom == -1
								))
						{
							if(arrRect.get(i).left <= -1 || arrRect.get(i).top <= -1
									||arrRect.get(i).bottom <= -1||arrRect.get(i).right <= -1)
								continue;
							if(arrRect.get(i).top<300){
								arrRect.get(i).top = arrRect.get(i).top;
							}
							if(nLeft!=-1) nLeft = toRealX(nLeft);
							if(nRight!=-1) nRight = toRealX(nRight);
							if(nTop!=-1) nTop = toRealY(nTop);
							if(nBottom!=-1) nBottom = toRealY(nBottom);//nRight-nLeft,nBottom-nTop);//
							Rect rtBack = new Rect();
							rtBack.left = (nLeft<arrRect.get(i).left)?nLeft:arrRect.get(i).left;
							rtBack.left -= 6;
							if(rtBack.left<0) rtBack.left = 0;
							rtBack.right = (nRight>arrRect.get(i).right)?nRight:arrRect.get(i).right;
							rtBack.right+=2;
							rtBack.top = (nTop<arrRect.get(i).top)?nTop:arrRect.get(i).top;
							rtBack.top -= 2;
							if(rtBack.top<0) rtBack.top = 0;
							rtBack.bottom = arrRect.get(i).bottom;//(nBottom>arrRect.get(i).bottom)?nBottom:arrRect.get(i).bottom;//nRight-nLeft,nBottom-nTop);//
							rtBack.bottom+=2;
							if(maxBottomMove<0){
								rtBack.bottom+=maxBottomMove;
								rtBack.top+=maxBottomMove;
							}
							canvas.drawRect(rtBack, paint);
							windowDraw = true;
						}
						
					}
		    	}
			}
			//end draw
			canvas.drawBitmap(bitmap, 0, maxBottomMove, null);

	    	//canvas.restore();
	    }   
	}  
	
	/**
	 * OSMP+ closed caption settings interface.
	 * 
	 * 
	 * 
	 */
	public class voSubTitleFormatSettingImpl implements voSubTitleFormatSetting{
	    /**
	     * set font color.
	     *
	     * @param nFontColor the font color for text
	     * for example, we use "nFontColor = Color.argb(255, 180, 20,220)" to set value of nFontColor
	     */
		public void setFontColor(int nFontColor) {
			fontColor = nFontColor;
			fontColorEnable = true;
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
		}
	    /**
	     * set font color opacity rate.
	     *
	     * @param nAlpha the font color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
	     */
		public void setFontOpacity(int nAlpha)
		{
			fontColorOpacityRateEnable = true;
			nAlpha = checkAlpha( nAlpha);
			this.fontColorOpacityRate = nAlpha;
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
		}
	    /**
	     * set font size, it will be delete, and it will be replaced with setFontSize(float nFontSize).
	     *
	     * @param nSmallFontSize the small font size for text, the parameter value is between 0.5 and 2.0, 0.5 is the smallest, 2.0 is the biggest font.
	     * @param nMediumFontSize the medium font size for text, the parameter value is between 0.5 and 2.0, 0.5 is the smallest, 2.0 is the biggest font.
	     * @param nLargeFontSzie the large font size for text, the parameter value is between 0.5 and 2.0, 0.5 is the smallest, 2.0 is the biggest font.
	     */
		public void setFontSize(float nSmallFontSize, float nMediumFontSize, float nLargeFontSzie) {
			fontSizeMedium = nMediumFontSize;
			fontSizeEnable = true;
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
		}
	    /**
	     * set font size scale.
	     *
	     * @param nFontSize the font size scale for text, the parameter value is between 50 and 200, 50 is the smallest and is the 0.5 times of default font size, 200 is the biggest font and is the 2.0 times of default font size.
	     */
		public void setFontSizeScale(int nFontSize){
			if(nFontSize>200)
				nFontSize = 200;
			if(nFontSize<50)
				nFontSize = 50;
			setFontSize(nFontSize/100.0f);
		}
	    /**
	     * set font size scale, it will replaced with setFontSizeScale.
	     *
	     * @param nFontSize the font size for text, the parameter value is between 0.5 and 2.0, 0.5 is the smallest, 2.0 is the biggest font.
	     */
		public void setFontSize(float nFontSize) {
			setFontSize(nFontSize, nFontSize, nFontSize);
		}
	    /**
	     * set background color.
	     *
	     * @param nBackgroundColor the background color for text,
	     * for example, we use "nBackgroundColor = Color.argb(255, 180, 20,220)" to set value of nBackgroundColor
	     */
		public void setBackgroundColor(int nBackgroundColor) {
			backgroundColor = nBackgroundColor;
			backgroundColorEnable = true;
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
		}
	    /**
	     * set background color opacity rate.
	     *
	     * @param nAlpha the background color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
	     */
		public void setBackgroundOpacity(int nAlpha)
		{
			backgroundColorOpacityRateEnable = true;
			nAlpha = checkAlpha( nAlpha);
			this.backgroundColorOpacityRate = nAlpha;
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
		}
	    /**
	     * set window background color.
	     *
	     * @param nBackgroundColor the window background color for text
	     * for example, we use "nBackgroundColor = Color.argb(255, 180, 20,220)" to set value of nBackgroundColor
	     */
		public void setWindowColor(int nBackgroundColor) {
			windowBackgroundColor = nBackgroundColor;
			windowBackgroundColorEnable = true;
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
		}
	    /**
	     * set window background color opacity rate.
	     *
	     * @param nAlpha the window background color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
	     */
		public void setWindowOpacity(int nAlpha){
			windowBackgroundColorOpacityRateEnable = true;
			nAlpha = checkAlpha( nAlpha);
			this.windowBackgroundColorOpacityRate = nAlpha;
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
		}
	    /**
	     * set font as italic or not.
	     *
	     * @param nFontItalic if it is true, the font will be italic
	     */
		public void setFontItalic(boolean nFontItalic) {
			fontItalic = nFontItalic?1:0;
			fontItalicEnable = true;
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
		}
	    /**
	     * set font as bold or not.
	     *
	     * @param nBold if it is true, the font will be bold
	     */
		public void setFontBold(boolean nBold) {
			fontBold = nBold?1:0;
			fontBoldEnable = true;
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
		}
	    /**
	     * set font as underline or not.
	     *
	     * @param nFontUnderline if it is true, the font will be underline
	     */
		public void setFontUnderline(boolean nFontUnderline) {
			fontUnderline = nFontUnderline?1:0;
			fontUnderlineEnable = true;
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
		}
		public void setTypeface(Typeface tf){
			fontTypeface = tf;
			fontName = "";
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
			
		}
	    /**
	     * set font name.
	     *
	     * @param strFontName the font name for text, will be deleted next version
	     */
		public void setFontName(String strFontName) {
			boolean bCreate = false;
			if(mainActivity!=null){
				String filename = strFontName+".ttf";
		        try {
		            InputStream InputStreamis  = mainActivity.getAssets().open(filename);
		            if(InputStreamis!=null){
						fontTypeface = Typeface.createFromAsset(mainActivity.getAssets(),  
								strFontName+".ttf");
						voLog.v(TAG,"setFontName from Assets"); 
						bCreate = true;
						}
		        } catch (IOException e) {
		            //e.printStackTrace();
		        	voLog.v(TAG,"no font in Assets"); 
		        }
			}
			if(strFontName.compareToIgnoreCase("Dom")==0 && !bCreate)
				fontName = "Casual";
			else
				fontName = strFontName;
			String [] strArr = {"Default","Courier","Times New Roman","Helvetica","Arial","Casual","Coronet","Gothic"};
			if(!bCreate){
				boolean bFound = false;
				int n = strArr.length;
				for(int i=0; i<n;i++){
					if(fontName.compareToIgnoreCase(strArr[i])==0){
						bFound = true;
						break;
					}
				}
				if(!bFound){
		        	voLog.i(TAG, "Font Style is :%s, ui set to default.",fontName);				}
			}
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
			
		}
	    /**
	     * set font style, same as setFontName.
	     *
	     * @param style the font style for text, style is value of below:
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Default}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_Monospaced_with_serifs}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Default_Proportionally_spaced_with_serifs}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Default_Monospaced_without_serifs}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Default_Proportionally_spaced_without_serifs}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Default_Casual}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Default_Cursive}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Default_Small_capitals}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Monospaced}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_SansSerif}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Serif}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_ProportionalSansSerif}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_ProportionalSerif}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Times_New_Roman}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Courier}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Helvetica}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Arial}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Dom}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Coronet}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_Gothic}
	     * <li> {@link voSubTitleFormatSetting#VOOSMP_FontStyle_MonospaceSansSerif}
	     */
		public void setFontStyle(int style) {
			fontTypeface = fromID(style);
			fontName = "Default";
//			if(style == VOOSMP_Monospaced_with_serifs)
//				fontName = "Courier";
//			if(style == VOOSMP_FontStyle_Default_Proportionally_spaced_with_serifs)
//				fontName = "Times New Roman";
//			if(style == VOOSMP_FontStyle_Default_Monospaced_without_serifs)
//				fontName = "Helvetica";
//			if(style == VOOSMP_FontStyle_Default_Proportionally_spaced_without_serifs)
//				fontName = "Arial";
//			if(style == VOOSMP_FontStyle_Default_Casual)
//				fontName = "Dom";
//			if(style == VOOSMP_FontStyle_Default_Cursive)
//				fontName = "Coronet";
//			if(style == VOOSMP_FontStyle_Default_Small_capitals)
//				fontName = "Gothic";
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
		}
		
	    /**
	     * set the edge type of font.
	     *
	     * @param edgeType the edge type of font, 0:NONE,1:RAISED,2:DEPRESSED,3:UNIFORM,4:LEFT_DROP_SHADOW,5:RIGHT_DROP_SHADOW
	     */
		public void setEdgeType(int edgeType) {
			this.edgeType = edgeType;
			edgeTypeEnable = true;
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
		}
	    /**
	     * set font edge color.
	     *
	     * @param edgeColor the font edge color for text
	     * for example, we use "edgeColor = Color.argb(255, 180, 20,220)" to set value of edgeColor
	     */
		public void setEdgeColor(int edgeColor) {
			this.edgeColor = edgeColor;
			edgeColorEnable = true;
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
		}
	    /**
	     * set edge color opacity rate.
	     *
	     * @param nAlpha the edge color opacity rate, it is between 0 and 100, 0 is transparent, 100 is opacity 
	     */
		public void setEdgeOpacity(int nAlpha){
			edgeColorOpacityRateEnable = true;
			nAlpha = checkAlpha( nAlpha);
			this.edgeColorOpacityRate = nAlpha;
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
		}
		private int checkAlpha(int nAlpha)
		{
			if(nAlpha<0)
				nAlpha = 0;
			if(nAlpha>100)
				nAlpha = 100;
			nAlpha = (int)(nAlpha*2.55);
			voLog.v("CloseCaption","alpha = %d ", nAlpha); 
			return nAlpha;
		}
	    /**
	     * Reset all parameters to default status. Subtitle will be presented as subtitle stream specified, instead of user specified
	     */
		public void reset()
		{
			edgeTypeEnable = false;
			
			fontColorEnable = false;
			edgeColorEnable = false;
			backgroundColorEnable = false;
			windowBackgroundColorEnable = false;
			fontColorOpacityRateEnable = false;
			edgeColorOpacityRateEnable = false;
			backgroundColorOpacityRateEnable = false;
			windowBackgroundColorOpacityRateEnable = false;
			
			fontSizeEnable = false;
			fontItalicEnable = false;
			fontUnderlineEnable = false;
			fontBoldEnable = false;
			fontName= "";
			fontTypeface = null;
			
			if(textViewOfRows!=null){
				textViewOfRows.invalidate();
			}
			
		}
	    /**
	     * clear all rectangle settings of rows.
	     */
		//public void clearRowRectList() {
		//	rectList = null;
		//}
		
	    /**
	     * add rectangle settings of row.
	     *
	     * @param rt the rectangle information of row, 0=<rt.left<100 0=<rt.top<100 0<rt.right<=100 0<rt.bottom<=100 
	     */
		//public void addRowRect(Rect rt) {
		//	if(rectList == null)
		//	{
		//		rectList = new ArrayList<Rect>();
		//	}
		//	rectList.add(rt);
		//}
		private int converColor(int oldColor, int newColor, boolean enableColor){
			if(enableColor)
			{
				oldColor = (newColor&0x00ffffff)|(oldColor&0xff000000);
			}
			return oldColor;
		}
		private int converColorOpacityRate(int oldColor, int newColorOpacityRate, boolean enableColorOpacityRate){
			if(enableColorOpacityRate)
			{
				oldColor = ((newColorOpacityRate&0x00ff)*0x01000000)|(oldColor&0x00ffffff);
			}
			return oldColor;
		}
		//
		private ArrayList<Rect> rectList = null;
		private int edgeType = 0;
		
		private int edgeColor = 0;
		private int fontColor = 0;
		private int windowBackgroundColor = 0;
		private int backgroundColor = 0;
		private int edgeColorOpacityRate = 0;
		private int fontColorOpacityRate = 0;
		private int windowBackgroundColorOpacityRate = 0;
		private int backgroundColorOpacityRate = 0;
		
		private float fontSizeMedium = 0.75f;
		private int	fontItalic=0;
		private int	fontUnderline=0;
		private int	fontBold=1;
		private boolean edgeTypeEnable = false;
		
		private boolean fontColorEnable = false;
		private boolean edgeColorEnable = false;
		private boolean backgroundColorEnable = false;
		private boolean windowBackgroundColorEnable = false;
		private boolean fontColorOpacityRateEnable = false;
		private boolean edgeColorOpacityRateEnable = false;
		private boolean backgroundColorOpacityRateEnable = false;
		private boolean windowBackgroundColorOpacityRateEnable = false;
		
		private boolean fontSizeEnable = false;
		private boolean	fontItalicEnable = false;
		private boolean	fontUnderlineEnable = false;
		private boolean	fontBoldEnable = false;
		private String fontName = "";
		private Typeface fontTypeface = null;

	}

}
