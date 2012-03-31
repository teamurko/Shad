package crawler;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;
import java.net.URLConnection;

public class Utils {
	public static String getContent(URL url) {
        StringBuilder page = new StringBuilder ();
        BufferedReader in;
        try {
            URLConnection conn = url.openConnection();
            String contentType = conn.getContentType();
            if (contentType != null && contentType.startsWith("text/html")) {
                if (contentType.indexOf("charset=") == -1) {
                    in = new BufferedReader(new InputStreamReader(conn.getInputStream(), "UTF-8"));
                } else {
                    String encoding = contentType.substring(contentType.indexOf("charset=") + 8);
                    in = new BufferedReader(new InputStreamReader(conn.getInputStream(), encoding));
                }
                String str;
                while ((str = in.readLine()) != null) {
                    page.append(str);
                }
                in.close();
                return page.toString();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }
}
