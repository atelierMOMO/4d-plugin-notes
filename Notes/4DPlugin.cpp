/* --------------------------------------------------------------------------------
 #
 #	4DPlugin.cpp
 #	source generated by 4D Plugin Wizard
 #	Project : Notes
 #	author : miyako
 #	2017/09/06
 #
 # --------------------------------------------------------------------------------*/


#include "4DPluginAPI.h"
#include "4DPlugin.h"

#pragma mark DateFormatter

namespace DateFormatter
{
	NSDateFormatter *ISO;
	NSDateFormatter *GMT;
	
	void setup()
	{
		GMT = [[NSDateFormatter alloc]init];
		[GMT setDateFormat:DATE_FORMAT_ISO_GMT];
		[GMT setTimeZone:[NSTimeZone timeZoneForSecondsFromGMT:0]];
		
		ISO = [[NSDateFormatter alloc]init];
		[ISO setDateFormat:DATE_FORMAT_ISO];
		[ISO setTimeZone:[NSTimeZone localTimeZone]];
	}
	
	void clear()
	{
		[GMT release];
		[ISO release];
	}
	
}

#pragma mark JSON

#if VERSIONMAC
NSString *json_conv(json_char *src)
{
	C_TEXT t;
	
	if(src)
	{
		std::wstring wstr = std::wstring(src);
		
		uint32_t dataSize = (uint32_t)((wstr.length() * sizeof(PA_Unichar)) + sizeof(PA_Unichar));
		std::vector<char> buf(dataSize);
		
		PA_ConvertCharsetToCharset((char *)wstr.c_str(),
															 (PA_long32)(wstr.length() * sizeof(wchar_t)),
															 eVTC_UTF_32,
															 (char *)&buf[0],
															 dataSize,
															 eVTC_UTF_16);
		
		//don't use len, null chars matter
		CUTF16String u16 = (const PA_Unichar *)&buf[0];
		t.setUTF16String(&u16);
	}
	return t.copyUTF16String();
}

NSString *json_copy_text(JSONNODE *n, const wchar_t *name)
{
	NSString *t = nil;
	
	if(n)
	{
		if(name)
		{
			json_char *json_string = json_as_string(json_get(n, name));
			t = json_conv((json_char *)json_string);
			json_free(json_string);
		}
	}
	return t ? t : @"";
}

void json_set_text(JSONNODE *n, NSString *value)
{
	if((n))
	{
		C_TEXT t;
		t.setUTF16String(value);
		
		uint32_t dataSize = (t.getUTF16Length() * sizeof(wchar_t))+ sizeof(wchar_t);
		std::vector<char> buf(dataSize);
		PA_ConvertCharsetToCharset((char *)t.getUTF16StringPtr(),
															 t.getUTF16Length() * sizeof(PA_Unichar),
															 eVTC_UTF_16,
															 (char *)&buf[0],
															 dataSize,
															 eVTC_UTF_32);
		JSONNODE *a = json_new(JSON_STRING);
		json_set_a(a, (wchar_t *)&buf[0]);
		json_push_back(n, a);
	}
}

void json_set_text(JSONNODE *n, const wchar_t *name, NSString *value)
{
	if((n))
	{
		if(name)
		{
			C_TEXT t;
			t.setUTF16String(value);
			
			uint32_t dataSize = (t.getUTF16Length() * sizeof(wchar_t))+ sizeof(wchar_t);
			std::vector<char> buf(dataSize);
			PA_ConvertCharsetToCharset((char *)t.getUTF16StringPtr(),
																 t.getUTF16Length() * sizeof(PA_Unichar),
																 eVTC_UTF_16,
																 (char *)&buf[0],
																 dataSize,
																 eVTC_UTF_32);
			
			JSONNODE *e = json_get(n, name);
			if(e)
			{
				json_set_a(e, (wchar_t *)&buf[0]);
			}else
			{
				json_push_back(n, json_new_a(name, (wchar_t *)&buf[0]));
			}
			
		}else
		{
			JSONNODE *node = json_new_a(name, L"");
			json_nullify(node);
			json_push_back(n, node);
		}
	}
}

void json_set_date(JSONNODE *n, const wchar_t *name, NSDate *value, BOOL isGMT = YES)
{
	if(isGMT)
	{
		json_set_text(n, name, value ? [DateFormatter::GMT stringFromDate:value] : @"");
	}else
	{
		json_set_text(n, name, value ? [DateFormatter::ISO stringFromDate:value] : @"");
	}
}

#endif

void json_set_text(JSONNODE *n, const wchar_t *name, C_TEXT &t)
{
	if(n)
	{
		if(name)
		{
			uint32_t dataSize = (t.getUTF16Length() * sizeof(wchar_t))+ sizeof(wchar_t);
			std::vector<char> buf(dataSize);
			PA_ConvertCharsetToCharset((char *)t.getUTF16StringPtr(),
																 t.getUTF16Length() * sizeof(PA_Unichar),
																 eVTC_UTF_16,
																 (char *)&buf[0],
																 dataSize,
																 eVTC_UTF_32);
			
			JSONNODE *e = json_get(n, name);
			if(e)
			{
				json_set_a(e, (wchar_t *)&buf[0]);
			}else
			{
				json_push_back(n, json_new_a(name, (wchar_t *)&buf[0]));
			}

		}else
		{
			JSONNODE *node = json_new_a(name, L"");
			json_nullify(node);
			json_push_back(n, node);
		}
	}
}

void json_stringify(JSONNODE *json, C_TEXT &t, BOOL pretty = NO)
{
	json_char *json_string = pretty ? json_write_formatted(json) : json_write(json);
	std::wstring wstr = std::wstring(json_string);
	uint32_t dataSize = (uint32_t)((wstr.length() * sizeof(wchar_t))+ sizeof(PA_Unichar));
	std::vector<char> buf(dataSize);
	PA_ConvertCharsetToCharset((char *)wstr.c_str(),
														 (PA_long32)(wstr.length() * sizeof(wchar_t)),
														 eVTC_UTF_32,
														 (char *)&buf[0],
														 dataSize,
														 eVTC_UTF_16);
	CUTF16String u16 = (const PA_Unichar *)&buf[0];
	t.setUTF16String(&u16);
	json_free(json_string);
}

namespace SQL
{
	NSString *toString(const unsigned char *value)
	{
		return value ? [NSString stringWithUTF8String:(const char *)value] : @"";
	}
}

namespace Notes
{
	static NotesApplication *Application = [SBApplication applicationWithBundleIdentifier:NOTES_APP_ID];
	static NSString *absolutePath = [[NSWorkspace sharedWorkspace]absolutePathForAppBundleWithIdentifier:NOTES_APP_ID];
	static NSBundle *bundle = [NSBundle bundleWithPath:absolutePath];
	static NSRegularExpression *attachment_regex = [NSRegularExpression regularExpressionWithPattern:@"(?:x-coredata:\\/\\/).*\\/ICAttachment\\/p(\\d+)"
																																													 options:NSRegularExpressionCaseInsensitive
																																														 error:nil];
	const char *sqlPath;
	
	typedef struct
	{
		NotesNote *note;
		NSData *data;
		NSString *name;
	}add_attachment_ctx;
	
	NSString *mediaPath = nil;
	NSString *previewPath = nil;
	
	const char *SQL_GET_NOTE_ATTACHMENT_TYPE = "\
	SELECT \n\
	ZICCLOUDSYNCINGOBJECT.ZTYPEUTI, \n\
	ZICCLOUDSYNCINGOBJECT.ZIDENTIFIER \n\
	FROM \n\
	ZICCLOUDSYNCINGOBJECT \n\
	WHERE \n\
	ZICCLOUDSYNCINGOBJECT.Z_PK = ? \n\
	LIMIT 1;";
	
	const char *SQL_GET_NOTE_ATTACHMENT = "\
	SELECT \n\
	ZICCLOUDSYNCINGOBJECT.ZIDENTIFIER, \n\
	ZICCLOUDSYNCINGOBJECT.ZFILENAME \n\
	FROM \n\
	ZICCLOUDSYNCINGOBJECT, \n\
	ZICCLOUDSYNCINGOBJECT AS A \n\
	WHERE \n\
	A.Z_PK = ? \n\
	AND \n\
	ZICCLOUDSYNCINGOBJECT.Z_PK = A.ZMEDIA;";
	
	const char *SQL_GET_NOTE_ATTACHMENT_PREVIEW = "\
	SELECT \n\
	ZICCLOUDSYNCINGOBJECT.ZIDENTIFIER, \n\
	ZICCLOUDSYNCINGOBJECT.ZORIENTATION \n\
	FROM \n\
	ZICCLOUDSYNCINGOBJECT, \n\
	ZICCLOUDSYNCINGOBJECT AS A \n\
	WHERE \n\
	A.Z_PK = ? \n\
	AND \n\
	ZICCLOUDSYNCINGOBJECT.ZIDENTIFIER LIKE A.ZIDENTIFIER || '%';";
	
	//scpt
	NSString *script =
	@"on add_attachment(note_id, attachment_path) \n\
	tell application \"Notes\" \n\
	set the_note to note id note_id \n\
	set the_attachment to make new attachment at the_note with data attachment_path \n\
	end tell \n\
	end add_attachment";
	
	bool isScriptCompiled = NO;
	
	NSAppleScript *scriptObject = nil;
	
	//  this doesn't work...
	//	NSBundle *Bundle = [NSBundle bundleWithIdentifier:NOTES_APP_ID];
	
#pragma mark Notes
	
	void sql_find_file()
	{
		NSString *path = nil;
		
		NSArray *URLs = [[NSFileManager defaultManager]
										 URLsForDirectory:NSLibraryDirectory
										 inDomains:NSUserDomainMask];
		
		if(URLs && [URLs count])
		{
			NSURL *libraryURL = [URLs objectAtIndex:0];
			
			//V7
			NSURL *groupContainerURL = [libraryURL URLByAppendingPathComponent:@"Group Containers"];
			NSURL *sqlParentURL = [groupContainerURL URLByAppendingPathComponent:@"group.com.apple.notes"];
			NSURL *sqlURL = [sqlParentURL URLByAppendingPathComponent:@"NoteStore.sqlite"];
			NSURL *mediaURL = [sqlParentURL URLByAppendingPathComponent:@"Media"];
			NSURL *previewURL = [sqlParentURL URLByAppendingPathComponent:@"Previews"];
			
			path = [sqlURL path];
			
			mediaPath = [mediaURL path];
			previewPath = [previewURL path];
		}
		sqlPath = path ? [path UTF8String] : NULL;
	}
	
	void setup()
	{
		scriptObject = [[NSAppleScript alloc]initWithSource:script];
		isScriptCompiled = [scriptObject compileAndReturnError:nil];
		
		sql_find_file();
	}
	
	void clear()
	{
		[scriptObject release];
	}

#pragma mark UUID
	
	NSString *uuidString()
	{
		return [[[NSUUID UUID]UUIDString]stringByReplacingOccurrencesOfString:@"-" withString:@""];
	}
	
#pragma mark Private
	
	NotesNote *_getNote(JSONNODE *json)
	{
		NotesNote *note = nil;
		
		SBElementArray *notes = [Notes::Application notes];
		
		NSArray *noteIds = [notes arrayByApplyingSelector:@selector(id)];
		
		if([noteIds count])
		{
			NSString *noteId = json_copy_text(json, L"id");
			
			NSUInteger i = [noteIds indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop)
											{
												if ([(NSString*)obj isEqualToString:noteId])
												{
													return YES;
												}
												return NO;
											}];
			
			if(NSNotFound != i)
			{
				note = [notes objectWithID:noteId];
			}
			[noteId release];
		}
		
		return note;
	}
	
	NotesFolder *_getFolder(JSONNODE *json)
	{
		NotesFolder *folder = nil;
		
		SBElementArray *folders = [Notes::Application folders];
		
		NSArray *folderIds = [folders arrayByApplyingSelector:@selector(id)];
		
		if([folderIds count])
		{
			NSString *folderId = json_copy_text(json, L"id");
			
			NSUInteger i = [folderIds indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop)
											{
												if ([(NSString*)obj isEqualToString:folderId])
												{
													return YES;
												}
												return NO;
											}];
			
			if(NSNotFound != i)
			{
				folder = [folders objectWithID:folderId];
			}
			[folderId release];
		}
		
		return folder;
	}
	
	NotesAccount *_getAccount(JSONNODE *json)
	{
		NotesAccount *account = nil;
		
		SBElementArray *accounts = [Notes::Application accounts];
		
		NSArray *accountIds = [accounts arrayByApplyingSelector:@selector(id)];
		
		if([accountIds count])
		{
			NSString *accountId = json_copy_text(json, L"id");
			
			NSUInteger i = [accountIds indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop)
											{
												if ([(NSString*)obj isEqualToString:accountId])
												{
													return YES;
												}
												return NO;
											}];
			
			if(NSNotFound != i)
			{
				account = [accounts objectWithID:accountId];
			}
			[accountId release];
		}
		
		return account;
	}

	/*
	void _getFolders(JSONNODE *json)
	{
		JSONNODE *folders_array = json_new(JSON_ARRAY);
		
		SBElementArray *folders = [Notes::Application folders];
		NSArray *folderNames = [folders valueForKey:@"name"];
		NSArray *folderIds = [folders valueForKey:@"id"];
		for(NSUInteger i = 0; i < [folderIds count]; ++i)
		{
			JSONNODE *item = json_new(JSON_NODE);
			
			NSString *folderName = NSLocalizedStringFromTableInBundle([folderNames objectAtIndex:i], nil, bundle, nil);
			NSString *folderId = [folderIds objectAtIndex:i];
			
			json_set_text(item, L"name", folderName);
			json_set_text(item, L"id", folderId);
			
			json_push_back(folders_array, item);
		}
		json_set_name(folders_array, L"folders");
		json_push_back(json, folders_array);
	}
	*/
	
	void _getFolder(JSONNODE *n, NotesFolder *folder)
	{
		JSONNODE *json = json_new(JSON_NODE);
		json_set_text(json, L"name", folder.name);
		json_set_text(json, L"id", folder.id);
		JSONNODE *folders_array = json_new(JSON_ARRAY);
		
		NSArray *folders = [folder folders];
		for(NSUInteger i = 0; i < [folders count]; ++i)
		{
			_getFolder(folders_array, [folders objectAtIndex:i]);
		}
		
		json_set_name(folders_array, L"folders");
		json_push_back(json, folders_array);
		json_push_back(n, json);
	}
	
	void _createNote(JSONNODE *json)
	{
		NotesFolder *folder = _getFolder(json);
		
		if(folder)
		{
			NSString *title = json_copy_text(json, L"name");
			NSString *body = json_copy_text(json, L"body");
			
			NSDictionary *notes_props = [[NSDictionary alloc]initWithObjectsAndKeys:
																	 title, @"name",
																	 body, @"body",
																	 nil];
			[title release];
			[body release];
			
			NotesNote *note = [[[Notes::Application classForScriptingClass:@"note"]alloc]
												 initWithProperties:notes_props];
			[notes_props release];
			
			[[folder notes]addObject:note];
			
			json_set_text(json, L"id", note.id);
			json_set_date(json, L"creationDate", note.creationDate);
			json_set_date(json, L"modificationDate", note.modificationDate);
			json_set_date(json, L"creationLocalDate", note.creationDate, NO);
			json_set_date(json, L"modificationLocalDate", note.modificationDate, NO);
			
			[note release];
		}
		
	}
	
#pragma mark Public
	
	void addAttachment(NotesNote *note, NSData *data, NSString *name)
	{
		NSArray *URLs = [[NSFileManager defaultManager]
										 URLsForDirectory:NSLibraryDirectory
										 inDomains:NSUserDomainMask];
		
		if(URLs && [URLs count])
		{
			NSURL *libraryDirectory = [URLs objectAtIndex:0];
			NSURL *containerDirectory = [libraryDirectory URLByAppendingPathComponent:@"Group Containers"];
			NSURL *groupDirectory = [containerDirectory URLByAppendingPathComponent:@"group.com.apple.notes"];
			NSURL *mediaDirectory = [groupDirectory URLByAppendingPathComponent:@"Media"];
			NSURL *uniqueDirectory = [mediaDirectory URLByAppendingPathComponent:uuidString()];
			NSURL *dstPath = [uniqueDirectory URLByAppendingPathComponent:name];
			
			if([[NSFileManager defaultManager]createDirectoryAtURL:uniqueDirectory
																 withIntermediateDirectories:YES attributes:nil error:nil])
			{
				if([data writeToURL:dstPath atomically:NO])
				{
					
					/*
					 NSString *script = [[NSString alloc]initWithFormat:
					 @"tell application \"%@\" \n\
					 set %@ to note id \"%@\" \n\
					 set %@ to make new attachment at %@ with data \"%@\" \n\
					 end tell",
					 @"Notes",
					 @"the_note", note.id,
					 @"the_attachment", @"the_note", [dstPath path]];
					 
					 NSLog(@"%@", script);
					 
					 NSAppleScript *scriptObject = [[NSAppleScript alloc]initWithSource:script];
					 
					 if([scriptObject compileAndReturnError:nil])
					 {
						NSAppleEventDescriptor *returnValue = [scriptObject executeAndReturnError:nil];
						NSLog(@"%@", returnValue);
					 }
					 [scriptObject release];
					 [script release];
					 */
					
					NSAppleEventDescriptor *parameters = [NSAppleEventDescriptor listDescriptor];
					NSAppleEventDescriptor *param_note_id = [NSAppleEventDescriptor descriptorWithString:note.id];
					[parameters insertDescriptor:param_note_id atIndex:1];
					NSAppleEventDescriptor *param_attachment_path = [NSAppleEventDescriptor descriptorWithString:[dstPath path]];
					[parameters insertDescriptor:param_attachment_path atIndex:2];
					
					ProcessSerialNumber psn = {0, kCurrentProcess};
					NSAppleEventDescriptor *target =
					[NSAppleEventDescriptor
					 descriptorWithDescriptorType:typeProcessSerialNumber
					 bytes:&psn
					 length:sizeof(ProcessSerialNumber)];
					
					NSAppleEventDescriptor *handler = [NSAppleEventDescriptor descriptorWithString:@"add_attachment"];
					
					NSAppleEventDescriptor *event =
					[NSAppleEventDescriptor appleEventWithEventClass:kASAppleScriptSuite
																									 eventID:kASSubroutineEvent
																					targetDescriptor:target
																									returnID:kAutoGenerateReturnID
																						 transactionID:kAnyTransactionID];
					
					[event setParamDescriptor:handler forKeyword:keyASSubroutineName];
					[event setParamDescriptor:parameters forKeyword:keyDirectObject];
					
					if(isScriptCompiled)
					{
						[scriptObject executeAppleEvent:event error:nil];
					}
					
					//cleanup
					if([[NSFileManager defaultManager]removeItemAtPath:[dstPath path] error:nil])
					{
						[[NSFileManager defaultManager]removeItemAtPath:[uniqueDirectory path] error:nil];
					}
				}
			}
		}
	}
	
	void addAttachment2(add_attachment_ctx *ctx)
	{
		addAttachment(ctx->note, ctx->data, ctx->name);
	}
	
	/*
	void getFolder(C_TEXT &folder)
	{
		JSONNODE *json = json_new(JSON_NODE);
		json_set_text(json, L"folder", folder);
		_getFolder(json);
		json_delete(json);
	}
	*/
	 
	/*
	void getFolders(C_TEXT &folders)
	{
		JSONNODE *json = json_new(JSON_NODE);
		_getFolders(json);
		json_stringify(json, folders);
		json_delete(json);
	}
	 */
	
	/*
	void getFolder(C_TEXT &folder_id, C_TEXT &folder_json)
	{
		JSONNODE *json = json_new(JSON_NODE);
		json_set_text(json, L"folder", folder_id);
		
		NotesFolder *folder = _getFolder(json);
		json_delete(json);
		
		json = json_new(JSON_NODE);
		if(folder)
		{
			//folder contains more folders and notes
			_getFolderContent(json, folder);
			
		}
		json_stringify(json, folder_json);
		json_delete(json);
	}
	*/
	
#pragma mark List
	
	void getNotes(ARRAY_TEXT &names)
	{
		JSONNODE *json = json_new(JSON_NODE);
		JSONNODE *notes_array = json_new(JSON_ARRAY);
		
		SBElementArray *notes = [Notes::Application notes];
		NSArray *noteNames = [notes valueForKey:@"name"];
		NSArray *noteIds = [notes valueForKey:@"id"];
		
		names.setSize(1);
		
		for(NSUInteger i = 0; i < [noteIds count]; ++i)
		{
			JSONNODE *item = json_new(JSON_NODE);
			
			NSString *noteName = [noteNames objectAtIndex:i];
			NSString *noteId = [noteIds objectAtIndex:i];
			
			json_set_text(item, L"name", noteName);
			json_set_text(item, L"id", noteId);
			
			NotesNote *note = [notes objectAtIndex:i];
			
			JSONNODE *attachments_array = json_new(JSON_ARRAY);
			NSArray *attachmentIds = [[note attachments]arrayByApplyingSelector:@selector(id)];
			for(NSUInteger j = 0; j < [attachmentIds count]; ++j)
			{
				json_set_text(attachments_array, [attachmentIds objectAtIndex:j]);
			}
			json_set_name(attachments_array, L"attachments");
			json_push_back(item, attachments_array);
			
			json_push_back(notes_array, item);
			names.appendUTF16String(noteName);
		}
		
		json_set_name(notes_array, L"notes");
		json_push_back(json, notes_array);
		
		C_TEXT notes_json;
		json_stringify(json, notes_json);
		json_delete(json);
		
		names.setUTF16StringAtIndex(notes_json.getUTF16StringPtr(), notes_json.getUTF16Length(), 0);
	}
	
	void getFolders(ARRAY_TEXT &names)
	{
		JSONNODE *json = json_new(JSON_NODE);
		JSONNODE *folders_array = json_new(JSON_ARRAY);
		
		SBElementArray *folders = [Notes::Application folders];
		NSArray *folderNames = [folders valueForKey:@"name"];
		NSArray *folderIds = [folders valueForKey:@"id"];
		
		names.setSize(1);
		
		for(NSUInteger i = 0; i < [folderIds count]; ++i)
		{
			JSONNODE *item = json_new(JSON_NODE);
			
			NSString *folderName = [folderNames objectAtIndex:i];
			NSString *folderId = [folderIds objectAtIndex:i];
			
			json_set_text(item, L"name", folderName);
			json_set_text(item, L"id", folderId);
			
			NotesFolder *folder = [folders objectAtIndex:i];
			
			JSONNODE *notes_array = json_new(JSON_ARRAY);
			NSArray *noteIds = [[folder notes]arrayByApplyingSelector:@selector(id)];
			for(NSUInteger j = 0; j < [noteIds count]; ++j)
			{
				json_set_text(notes_array, [noteIds objectAtIndex:j]);
			}
			json_set_name(notes_array, L"notes");
			json_push_back(item, notes_array);

			JSONNODE *subfolders_array = json_new(JSON_ARRAY);
			NSArray *subfolderIds = [[folder folders]arrayByApplyingSelector:@selector(id)];
			for(NSUInteger j = 0; j < [subfolderIds count]; ++j)
			{
				json_set_text(subfolders_array, [subfolderIds objectAtIndex:j]);
			}
			json_set_name(subfolders_array, L"folders");
			json_push_back(item, subfolders_array);
			
			json_push_back(folders_array, item);
			names.appendUTF16String(folderName);
		}
		
		json_set_name(folders_array, L"folders");
		json_push_back(json, folders_array);
		
		C_TEXT folders_json;
		json_stringify(json, folders_json);
		json_delete(json);
		
		names.setUTF16StringAtIndex(folders_json.getUTF16StringPtr(), folders_json.getUTF16Length(), 0);
	}
	
	void getAccounts(ARRAY_TEXT &names)
	{
		JSONNODE *json = json_new(JSON_NODE);
		JSONNODE *accounts_array = json_new(JSON_ARRAY);
		
		SBElementArray *accounts = [Notes::Application accounts];
		NSArray *accountNames = [accounts valueForKey:@"name"];
		NSArray *accountIds = [accounts valueForKey:@"id"];
		
		names.setSize(1);
		
		for(NSUInteger i = 0; i < [accountIds count]; ++i)
		{
			JSONNODE *item = json_new(JSON_NODE);
			
			NSString *accountName = [accountNames objectAtIndex:i];
			NSString *accountId = [accountIds objectAtIndex:i];
			
			json_set_text(item, L"name", accountName);
			json_set_text(item, L"id", accountId);
			
			NotesAccount *account = [accounts objectAtIndex:i];
			
			JSONNODE *folders_array = json_new(JSON_ARRAY);
			NSArray *folderIds = [[account folders]arrayByApplyingSelector:@selector(id)];
			for(NSUInteger j = 0; j < [folderIds count]; ++j)
			{
				json_set_text(folders_array, [folderIds objectAtIndex:j]);
			}
			json_set_name(folders_array, L"folders");
			json_push_back(item, folders_array);
			
			json_push_back(accounts_array, item);
			names.appendUTF16String(accountName);
		}
		
		json_set_name(accounts_array, L"accounts");
		json_push_back(json, accounts_array);
		
		C_TEXT accounts_json;
		json_stringify(json, accounts_json);
		json_delete(json);
		
		names.setUTF16StringAtIndex(accounts_json.getUTF16StringPtr(), accounts_json.getUTF16Length(), 0);
	}
	
	void getAttachments(ARRAY_TEXT &names)
	{
		JSONNODE *json = json_new(JSON_NODE);
		JSONNODE *attachments_array = json_new(JSON_ARRAY);
		
		SBElementArray *attachments = [Notes::Application attachments];
		NSArray *attachmentNames = [attachments valueForKey:@"name"];
		NSArray *attachmentIds = [attachments valueForKey:@"id"];
		
		names.setSize(1);
		
		for(NSUInteger i = 0; i < [attachmentIds count]; ++i)
		{
			JSONNODE *item = json_new(JSON_NODE);
			
			NSString *attachmentName = [attachmentNames objectAtIndex:i];
			NSString *attachmentId = [attachmentIds objectAtIndex:i];
			
			json_set_text(item, L"name", attachmentName);
			json_set_text(item, L"id", attachmentId);
			
			NotesAttachment *attachment = [attachments objectAtIndex:i];
			json_set_text(item, L"note", attachment.id);
			
			json_push_back(attachments_array, item);
			names.appendUTF16String(attachmentName);
		}
		
		json_set_name(attachments_array, L"attachments");
		json_push_back(json, attachments_array);
		
		C_TEXT attachments_json;
		json_stringify(json, attachments_json);
		json_delete(json);
		
		names.setUTF16StringAtIndex(attachments_json.getUTF16StringPtr(), attachments_json.getUTF16Length(), 0);
	}
	
#pragma mark Item
	
	void getNote(C_TEXT &note_id, C_TEXT &note_json)
	{
		JSONNODE *json = json_new(JSON_NODE);
		json_set_text(json, L"id", note_id);
		NotesNote *note = _getNote(json);
		
		if(note)
		{
			NotesFolder *folder = note.container.get;
			json_set_text(json, L"folder", folder.id);
			json_set_text(json, L"name", note.name);
			json_set_text(json, L"body", note.body);
			json_set_date(json, L"creationDate", note.creationDate);
			json_set_date(json, L"modificationDate", note.modificationDate);
			json_set_date(json, L"creationLocalDate", note.creationDate, NO);
			json_set_date(json, L"modificationLocalDate", note.modificationDate, NO);
			
			//attachments
			JSONNODE *attachments_array = json_new(JSON_ARRAY);
			NSArray *attachmentIds = [[note attachments]arrayByApplyingSelector:@selector(id)];
			for(NSUInteger i = 0; i < [attachmentIds count]; ++i)
			{
				json_set_text(attachments_array, [attachmentIds objectAtIndex:i]);
			}
			json_set_name(attachments_array, L"attachments");
			json_push_back(json, attachments_array);
		}else
		{
			json_delete(json);
			json = json_new(JSON_NODE);
		}
		
		json_stringify(json, note_json);
		json_delete(json);
	}
	
	void getFolder(C_TEXT &folder_id, C_TEXT &note_json)
	{
		JSONNODE *json = json_new(JSON_NODE);
		json_set_text(json, L"id", folder_id);
		
		NotesFolder *folder = _getFolder(json);
		
		if(folder)
		{
			json_set_text(json, L"name", folder.name);
			json_set_text(json, L"id", folder.id);
			
			NSArray *folders = [folder folders];
			
			JSONNODE *folders_array = json_new(JSON_ARRAY);
			for(NSUInteger i = 0; i < [folders count]; ++i)
			{
				_getFolder(folders_array, [folders objectAtIndex:i]);
			}
			json_set_name(folders_array, L"folders");
			json_push_back(json, folders_array);
			
			JSONNODE *notes_array = json_new(JSON_ARRAY);
			NSArray *noteIds = [[folder notes]arrayByApplyingSelector:@selector(id)];
			NSArray *noteNames = [[folder notes]arrayByApplyingSelector:@selector(name)];
			
			for(NSUInteger j = 0; j < [noteIds count]; ++j)
			{
				JSONNODE *item = json_new(JSON_NODE);
				json_set_text(item, L"name", [noteNames objectAtIndex:j]);
				json_set_text(item, L"id", [noteIds objectAtIndex:j]);
				json_push_back(notes_array, item);
			}
			json_set_name(notes_array, L"notes");
			json_push_back(json, notes_array);
			
		}else
		{
			json_delete(json);
			json = json_new(JSON_NODE);
		}
		
		json_stringify(json, note_json);
		json_delete(json);
	}
	
	void getAccount(C_TEXT &account_id, C_TEXT &account_json)
	{
		JSONNODE *json = json_new(JSON_NODE);
		json_set_text(json, L"id", account_id);
		NotesAccount *account = _getAccount(json);
		
		if(account)
		{
			json_set_text(json, L"id", account.id);
			json_set_text(json, L"name", account.name);
			
			JSONNODE *folders_array = json_new(JSON_ARRAY);
			
			NSArray *folders = [account folders];
			for(NSUInteger i = 0; i < [folders count]; ++i)
			{
				_getFolder(folders_array, [folders objectAtIndex:i]);
			}
			
			json_set_name(folders_array, L"folders");
			json_push_back(json, folders_array);
			
		}else
		{
			json_delete(json);
			json = json_new(JSON_NODE);
		}
		
		json_stringify(json, account_json);
		json_delete(json);
	}
	
#pragma mark -
	
	void createFolder(C_TEXT &parent_id, C_TEXT &name, C_TEXT &folder_json)
	{
		JSONNODE *json = json_new(JSON_NODE);
		json_set_text(json, L"id", parent_id);
		NotesFolder *folder = _getFolder(json);
		
		SBObject *parent = nil;
		
		if(name.getUTF16Length())
		{
			if(folder)
			{
				parent = folder;
			}else
			{
				NotesAccount *account = _getAccount(json);
				
				if(account)
				{
					parent = account;
				}
			}
		}

		if(parent)
		{
			NSString *title = name.copyUTF16String();
			NSDictionary *folders_props = [[NSDictionary alloc]initWithObjectsAndKeys:
																		 title, @"name",
																		 nil];
			[title release];
			
			NotesFolder *folder = [[[Notes::Application classForScriptingClass:@"folder"]alloc]
														 initWithProperties:folders_props];
			[folders_props release];
			
			[[parent folders]addObject:folder];
			json_set_text(json, L"id", folder.id);
			json_set_text(json, L"name", folder.name);
			
			[folder release];
		}else
		{
			json_delete(json);
			json = json_new(JSON_NODE);
		}
		
		json_stringify(json, folder_json);
		json_delete(json);
	}
	
	void createNote(PackagePtr pParams, sLONG_PTR *pResult)
	{
		C_TEXT folder;
		C_TEXT title;
		C_TEXT body;
		C_TEXT result;
		
		folder.fromParamAtIndex(pParams, 1);
		title.fromParamAtIndex(pParams, 2);
		body.fromParamAtIndex(pParams, 3);
		
		JSONNODE *json = json_new(JSON_NODE);
		json_set_text(json, L"id", folder);
		json_set_text(json, L"name", title);
		json_set_text(json, L"body", body);
		
		_createNote(json);
		
		NotesNote *note = _getNote(json);
		
		if(note)
		{
			PA_Variable *attachments = ((PA_Variable *)pParams[3]);//$4
			PA_Variable *filenames = ((PA_Variable *)pParams[4]);//$5
			
			if((attachments->fType == eVK_ArrayBlob) && (filenames->fType == eVK_ArrayUnicode))
			{
				unsigned int param4_count = PA_GetArrayNbElements(*attachments);
				unsigned int param5_count = PA_GetArrayNbElements(*filenames);
				
				if(param4_count == param5_count)
				{
					for(unsigned int i = 0; i <= param4_count; ++i)
					{
						//core animation problem..?
						PA_YieldAbsolute();
						
						PA_Blob param4_blob = PA_GetBlobInArray(*attachments, i);
						const void *param4_buf = (const void *)PA_LockHandle(param4_blob.fHandle);
						NSUInteger param4_len = (NSUInteger)param4_blob.fSize;
						if(param4_len)
						{
							//4D crashes; need to copy..?
							
							C_BLOB buf;
							buf.setBytes((const uint8_t *)param4_buf, (unsigned int)param4_len);
							NSData *data = [[NSData alloc]initWithBytes:buf.getBytesPtr() length:buf.getBytesLength()];
//							NSData *data = [[NSData alloc]initWithBytes:param4_buf length:param4_len];
							
							PA_Unistring param5_text = PA_GetStringInArray(*filenames, i);
							
							//apparently this side is OK; because it is not a handle..?							
							NSString *name = [[NSString alloc]initWithCharacters:param5_text.fString length:param5_text.fLength];
							
							add_attachment_ctx ctx;
							ctx.data = data;
							ctx.name = name;
							ctx.note = note;
							
							//autorelease problem
//							PA_RunInMainProcess((PA_RunInMainProcessProcPtr)Notes::addAttachment2, &ctx);
							addAttachment(note, data, name);
							
							[name release];
							[data release];
						}
						PA_UnlockHandle(param4_blob.fHandle);
					}
				}
			}
		}else
		{
			json_delete(json);
			json = json_new(JSON_NODE);
		}
		
		json_stringify(json, result);
		json_delete(json);
		
		result.setReturn(pResult);
	}
}

#pragma mark -

void OnStartup()
{
	DateFormatter::setup();
	Notes::setup();
}

void OnExit()
{
	DateFormatter::clear();
	Notes::clear();
}

bool isProcessOnExit()
{
	C_TEXT name;
	PA_long32 state, time;
	PA_GetProcessInfo(PA_GetCurrentProcessNumber(), name, &state, &time);
	CUTF16String procName(name.getUTF16StringPtr());
	CUTF16String exitProcName((PA_Unichar *)"$\0x\0x\0\0\0");
	return (!procName.compare(exitProcName));
}

void OnCloseProcess()
{
	if(isProcessOnExit())
	{
		OnExit();
	}
}

#pragma mark -
void PluginMain(PA_long32 selector, PA_PluginParameters params)
{
	try
	{
		PA_long32 pProcNum = selector;
		sLONG_PTR *pResult = (sLONG_PTR *)params->fResult;
		PackagePtr pParams = (PackagePtr)params->fParameters;
		
		CommandDispatcher(pProcNum, pResult, pParams);
	}
	catch(...)
	{
		
	}
}

void CommandDispatcher (PA_long32 pProcNum, sLONG_PTR *pResult, PackagePtr pParams)
{
	switch(pProcNum)
	{
		case kInitPlugin :
		case kServerInitPlugin :
			OnStartup();
			break;
			
		case kCloseProcess :
			OnCloseProcess();
			break;
			// --- Notes
			
		case 1 :
			Notes_Create_note(pResult, pParams);
			break;
			
		case 2 :
			Notes_GET_FOLDERS(pResult, pParams);
			break;

		case 3 :
			Notes_GET_NOTES(pResult, pParams);
			break;

		case 4 :
			Notes_GET_ATTACHMENTS(pResult, pParams);
			break;

		case 5 :
			Notes_GET_ACCOUNTS(pResult, pParams);
			break;

		case 6 :
			Notes_Create_folder(pResult, pParams);
			break;

		case 7 :
			Notes_SET_NOTIFICATION(pResult, pParams);
			break;

		case 8 :
			Notes_Get_notification(pResult, pParams);
			break;

		case 9 :
			Notes_Get_attachment(pResult, pParams);
			break;

		case 10 :
			Notes_Get_note(pResult, pParams);
			break;

		case 11 :
			Notes_Get_folder(pResult, pParams);
			break;

		case 12 :
			Notes_Get_account(pResult, pParams);
			break;

	}
}

// ------------------------------------- Notes ------------------------------------

#pragma mark -

void Notes_Create_note(sLONG_PTR *pResult, PackagePtr pParams)
{
	Notes::createNote(pParams, pResult);
}

void Notes_Create_folder(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	Notes::createFolder(Param1, Param2, returnValue);
	
	returnValue.setReturn(pResult);
}

#pragma mark List

void Notes_GET_NOTES(sLONG_PTR *pResult, PackagePtr pParams)
{
	ARRAY_TEXT Param1;
	
	Notes::getNotes(Param1);
	
	Param1.toParamAtIndex(pParams, 1);
}

void Notes_GET_FOLDERS(sLONG_PTR *pResult, PackagePtr pParams)
{
	ARRAY_TEXT Param1;

	Notes::getFolders(Param1);

	Param1.toParamAtIndex(pParams, 1);
}

void Notes_GET_ACCOUNTS(sLONG_PTR *pResult, PackagePtr pParams)
{
	ARRAY_TEXT Param1;

	Notes::getAccounts(Param1);
	
	Param1.toParamAtIndex(pParams, 1);
}

void Notes_GET_ATTACHMENTS(sLONG_PTR *pResult, PackagePtr pParams)
{
	ARRAY_TEXT Param1;
	
	Notes::getAttachments(Param1);
	
	Param1.toParamAtIndex(pParams, 1);
}

#pragma mark Item

void Notes_Get_note(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	
	Notes::getNote(Param1, returnValue);
	
	returnValue.setReturn(pResult);
}

void Notes_Get_folder(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	
	Notes::getFolder(Param1, returnValue);
	
	returnValue.setReturn(pResult);
}

void Notes_Get_account(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	
	Notes::getAccount(Param1, returnValue);
	
	returnValue.setReturn(pResult);
}

#pragma mark Item (SQL)

void Notes_Get_attachment(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	
	NSString *attachment_id = Param1.copyUTF16String();
	
	if(Notes::attachment_regex)
	{
		NSTextCheckingResult *match = [Notes::attachment_regex firstMatchInString:attachment_id
																 options:NSMatchingAnchored
																	 range:NSMakeRange(0, [attachment_id length])];
		if((match) && ([match numberOfRanges] > 1))
		{
			NSRange range = [match rangeAtIndex:1];
			NSString *Z_PK = [attachment_id substringWithRange:range];

			sqlite3 *notesStore = NULL;
			if(Notes::sqlPath)
			{
				if(SQLITE_OK == sqlite3_open(Notes::sqlPath, &notesStore))
				{
					CUTF8String attachment_id;
					Param1.copyUTF8String(&attachment_id);
					const char *PK = [Z_PK UTF8String];
					
					sqlite3_stmt *sql0 = NULL;
					if(SQLITE_OK == sqlite3_prepare_v2(notesStore, Notes::SQL_GET_NOTE_ATTACHMENT_TYPE, 1024, &sql0, NULL))
					{
						sqlite3_bind_text(sql0, 1, (const char *)PK, (int)strlen(PK), NULL);
						NSString *media_type = nil;
						while(SQLITE_ROW == sqlite3_step(sql0))
						{
							media_type = SQL::toString(sqlite3_column_text(sql0, 0));
						}
						if([media_type isEqualToString:@"com.apple.notes.sketch"])
						{
							sqlite3_stmt *sql2 = NULL;
							if(SQLITE_OK == sqlite3_prepare_v2(notesStore, Notes::SQL_GET_NOTE_ATTACHMENT_PREVIEW, 1024, &sql2, NULL))
							{
								sqlite3_bind_text(sql2, 1, (const char *)PK, (int)strlen(PK), NULL);
								bool with_orientation = NO;
								while(SQLITE_ROW == sqlite3_step(sql2))
								{
									NSString *preview_id = SQL::toString(sqlite3_column_text(sql2, 0));
									NSString *preview_orientation = SQL::toString(sqlite3_column_text(sql2, 1));
									if([preview_orientation isEqualToString:@"2"])
									{
										with_orientation = YES;
									}
									NSString *previewPath = [Notes::previewPath stringByAppendingPathComponent:preview_id];
									if(with_orientation)
									{
										NSString *mediaPath = [previewPath stringByAppendingString:@"-oriented.png"];
										returnValue.setPath(mediaPath);
									}else
									{
										NSString *mediaPath = [previewPath stringByAppendingString:@".png"];
										returnValue.setPath(mediaPath);
									}
								}
							}
						}else
						{
							sqlite3_stmt *sql1 = NULL;
							if(SQLITE_OK == sqlite3_prepare_v2(notesStore, Notes::SQL_GET_NOTE_ATTACHMENT, 1024, &sql1, NULL))
							{
								sqlite3_bind_text(sql1, 1, (const char *)PK, (int)strlen(PK), NULL);
								while(SQLITE_ROW == sqlite3_step(sql1))
								{
									NSString *media_folder_id = SQL::toString(sqlite3_column_text(sql1, 0));
									NSString *media_file_name = SQL::toString(sqlite3_column_text(sql1, 1));
									if([media_folder_id length] && [media_file_name length])
									{
										NSString *mediaFolderPath = [Notes::mediaPath stringByAppendingPathComponent:media_folder_id];
										NSString *mediaPath = [mediaFolderPath stringByAppendingPathComponent:media_file_name];
										returnValue.setPath(mediaPath);
									}
								}
							}
						}
					}
					sqlite3_close(notesStore);
				}
			}
		}
	}
	
	[attachment_id release];
	
	returnValue.setReturn(pResult);
}

#pragma mark -


void Notes_SET_NOTIFICATION(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;

	Param1.fromParamAtIndex(pParams, 1);


	
	
	

}

void Notes_Get_notification(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT returnValue;

	// --- write the code of Notes_Get_notification here...

	returnValue.setReturn(pResult);
}

